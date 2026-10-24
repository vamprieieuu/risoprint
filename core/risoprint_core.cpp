#include "risoprint_core.h"
#include <cstring>

namespace resoprint {

static inline Color sampleBilinear(const Image& im,float fx,float fy){
    fx=std::max(0.f,std::min((float)(im.w-1),fx));
    fy=std::max(0.f,std::min((float)(im.h-1),fy));
    int x0=(int)std::floor(fx), y0=(int)std::floor(fy); int x1=std::min(im.w-1,x0+1), y1=std::min(im.h-1,y0+1);
    float tx=fx-x0,ty=fy-y0;
    const float* a=im.at(x0,y0); const float* b=im.at(x1,y0); const float* c=im.at(x0,y1); const float* d=im.at(x1,y1);
    Color o;
    for(int k=0;k<3;k++){ float ab=a[k]+(b[k]-a[k])*tx; float cd=c[k]+(d[k]-c[k])*tx; (&o.r)[k]=ab+(cd-ab)*ty; }
    return o;
}

static inline Color sampleSoft(const Image& im,float fx,float fy,float radius){
    if(radius<0.35f) return sampleBilinear(im,fx,fy);
    Color o{0,0,0}; float wsum=0.f;
    int r=(int)std::ceil(radius*1.5f); float sigma=std::max(0.5f,radius*0.65f);
    for(int j=-r;j<=r;j++) for(int i=-r;i<=r;i++){
        float d2=(float)(i*i+j*j), w=std::exp(-d2/(2.f*sigma*sigma));
        Color s=sampleBilinear(im,fx+i,fy+j); o.r+=s.r*w;o.g+=s.g*w;o.b+=s.b*w;wsum+=w;
    }
    o.r/=wsum;o.g/=wsum;o.b/=wsum;return o;
}

static inline float hueAffinity(const Color& s,const Color& ink){
    float mx=std::max(s.r,std::max(s.g,s.b)), mn=std::min(s.r,std::min(s.g,s.b));
    float d=mx-mn;
    if(d<1e-5f) return 1.f;
    float h=0.f;
    if(mx==s.r) h=std::fmod((s.g-s.b)/d,6.f)/6.f;
    else if(mx==s.g) h=((s.b-s.r)/d+2.f)/6.f;
    else h=((s.r-s.g)/d+4.f)/6.f;
    if(h<0) h+=1.f;
    float imx=std::max(ink.r,std::max(ink.g,ink.b)), imn=std::min(ink.r,std::min(ink.g,ink.b));
    float id=imx-imn, ih=0.f;
    if(id>1e-5f){
        if(imx==ink.r) ih=std::fmod((ink.g-ink.b)/id,6.f)/6.f;
        else if(imx==ink.g) ih=((ink.b-ink.r)/id+2.f)/6.f;
        else ih=((ink.r-ink.g)/id+4.f)/6.f;
        if(ih<0) ih+=1.f;
    }
    float hd=std::fabs(h-ih); hd=std::min(hd,1.f-hd);
    float a=clampf(1.f-hd/0.33f);
    return a*a*(3.f-2.f*a);
}

static inline float grain(int x,int y,double scale,double t){
    // Two scales: a fine print grain and a slightly larger broken-paper component.
    float s=(float)std::max(0.035,scale);
    int gx=(int)std::floor(x/(s*5.5)); int gy=(int)std::floor(y/(s*5.5));
    float coarse=noise01(gx,gy,(int)std::floor(t*12.0));
    float fine=noise01(x,y,(int)std::floor(t*24.0));
    float mid=noise01((int)std::floor(x/(s*1.7)),(int)std::floor(y/(s*1.7)),17);
    return clampf(0.16f*coarse+0.58f*fine+0.26f*mid);
}

static inline float plateDensity(const Color& s,const Color& ink,double sep,double inkContrast,double contrast){
    // Build a real ink plate from source luminance + chroma/hue affinity.
    // Dark neutrals are shared by all plates; saturated colors preferentially feed
    // the closest ink. Separation controls the plate threshold rather than simply
    // scaling the final opacity.
    float lum=luma(s.r,s.g,s.b);
    float darkness=clampf(1.f-lum);
    float sat=clampf(std::max(s.r,std::max(s.g,s.b))-std::min(s.r,std::min(s.g,s.b)));
    float affinity=hueAffinity(s,ink);

    // Preserve dark structure while allowing the selected ink to dominate colored areas.
    float selective=(sat<0.10f)?1.f:(0.30f+0.70f*std::pow(affinity,2.2f));
    float d=darkness*(0.48f+0.52f*selective) + sat*0.34f*selective;

    // Contrast is centered at 1.0 so the reference default 1.58 visibly increases
    // separation without erasing the midtones.
    float c=(float)std::max(0.25,std::min(3.0,contrast));
    d=clampf((d-0.5f)*c+0.5f);

    // Higher Separation means the plate prints more readily. These thresholds keep
    // all three plates alive on dark/neutral regions while preserving hue selectivity.
    float sepv=(float)std::max(0.0,std::min(1.0,sep));
    // Keep the plate threshold in a usable midtone range. The previous high
    // thresholds made nearly every pixel fall through to white paper.
    float lo=0.42f-sepv*0.28f;
    float hi=0.88f-sepv*0.22f;
    d=smoothstep(lo,hi,d);

    // Ink contrast is a small final shaping control around the midpoint.
    float ic=(float)std::max(-1.0,std::min(1.0,inkContrast));
    d=clampf((d-0.5f)*(1.f+ic*0.55f)+0.5f);
    return d;
}

void render(const Image& src,const Image& dst,const Params& p,const FrameCtx& ctx){
    const int W=src.w,H=src.h;
    if(W<=0||H<=0) return;
    const float intensity=clampf((float)p.intensity);
    const float softRadius=(float)(p.softness*2.4);
    const float grainScale=(float)std::max(0.03,p.scale);
    const float grainVisibility=clampf((float)p.opacity);

    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            float* out=dst.at(x,y);
            Color paper=p.paper;
            if(p.showSource){ Color s=sampleBilinear(src,(float)x,(float)y); out[0]=s.r;out[1]=s.g;out[2]=s.b;out[3]=src.at(x,y)[3]; continue; }

            Color accum=paper;
            // Screened paper texture is shared by all plates, but each plate gets a slightly
            // different threshold so overlaps create the characteristic riso chatter.
            float baseGrain=grain(x,y,grainScale,ctx.timeSec);
            for(int k=0;k<3;k++){
                const InkParams& ip=p.ink[k];
                if(!ip.enabled) continue;
                float sx=(float)(x + ip.shiftX*W), sy=(float)(y + ip.shiftY*H);
                Color s=sampleSoft(src,sx,sy,softRadius);
                float d=plateDensity(s,ip.color,ip.separation,p.inkContrast,p.contrast);
                float n=noise01(x+k*4099,y+k*7919,(int)std::floor(ctx.timeSec*8.0));
                // Soft stochastic screen: more grain at the plate boundary and in highlights.
                float grainTerm=(baseGrain-0.5f)*0.52f + (n-0.5f)*0.18f;
                float thresholded=clampf(d + grainTerm*(0.42f + 0.58f*grainVisibility));
                // Do not require almost-black pixels before an ink plate appears.
                // This retains horse/fur detail and avoids an almost-white output.
                float coverage=smoothstep(0.015f,0.78f,thresholded);
                // Let the paper break through the ink according to opacity.
                coverage *= (1.f - grainVisibility*0.34f*baseGrain);
                // Normal subtractive plate: ink replaces paper in covered areas.
                // Opacity controls ink strength, but never cancels the plate entirely.
                float a=clampf(coverage*intensity*(0.45f+0.55f*grainVisibility)*0.92f);
                auto blendChannel=[&](float base,float ink)->float {
                    float mixed=ink;
                    switch(p.blend){
                        case 2: mixed=std::min(base,ink); break; // Darken
                        case 3: mixed=base*ink; break; // Multiply
                        case 4: mixed=(ink<=0.0001f)?0.f:1.f-std::min(1.f,(1.f-base)/ink); break; // Color Burn
                        case 5: mixed=std::max(base,ink); break; // Lighten
                        case 6: mixed=1.f-(1.f-base)*(1.f-ink); break; // Screen
                        case 7: mixed=std::min(1.f,base+ink); break; // Plus Lighter
                        case 8: mixed=(ink>=0.9999f)?1.f:std::min(1.f,base/(1.f-ink)); break; // Color Dodge
                        case 9: mixed=(base<0.5f)?(2.f*base*ink):(1.f-2.f*(1.f-base)*(1.f-ink)); break; // Overlay
                        case 10: mixed=(1.f-2.f*ink)*base*base+2.f*ink*base; break; // Soft Light approximation
                        case 11: mixed=(ink<0.5f)?(2.f*base*ink):(1.f-2.f*(1.f-base)*(1.f-ink)); break; // Hard Light
                        default: mixed=ink; break; // Normal
                    }
                    return clampf(base*(1.f-a)+mixed*a);
                };
                accum.r=blendChannel(accum.r,ip.color.r);
                accum.g=blendChannel(accum.g,ip.color.g);
                accum.b=blendChannel(accum.b,ip.color.b);
            }
            float srcA=src.at(x,y)[3];
            // Retain source micro-detail by gently mixing the print result back over the original.
            const float keep=clampf((float)p.blendOriginal);
            const float sr=src.at(x,y)[0], sg=src.at(x,y)[1], sb=src.at(x,y)[2];
            out[0]=clampf(accum.r*(1.f-keep)+sr*keep);
            out[1]=clampf(accum.g*(1.f-keep)+sg*keep);
            out[2]=clampf(accum.b*(1.f-keep)+sb*keep);
            if(p.blackWhite){
                const float gray=luma(out[0],out[1],out[2]);
                out[0]=out[1]=out[2]=gray;
            }
            out[3]=srcA;
        }
    }
}

} // namespace resoprint
