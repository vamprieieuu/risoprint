#include "resoprint_core.h"
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
    float lum=luma(s.r,s.g,s.b);
    float darkness=1.f-lum;
    float sat=std::max(s.r,std::max(s.g,s.b))-std::min(s.r,std::min(s.g,s.b));
    float affinity=hueAffinity(s,ink);
    // Risograph separation: dark neutrals can hit every plate, while saturated tones
    // preferentially select the plate whose ink hue is closest to the source color.
    float selective=(sat<0.18f)?1.f:std::pow(affinity,3.f);
    float d=std::pow(darkness,1.15f)*(0.62f+0.38f*selective)
           +(1.f-darkness)*std::pow(selective,1.5f)*0.42f;
    d=clampf((d-0.5f)*(float)contrast+0.5f);
    float sb=(float)(0.34+sep*0.72);
    d=clampf((d-(0.58f-sb*0.42f))/(0.58f+sb*0.08f));
    float ic=(float)inkContrast;
    d=clampf((d-0.5f)*(1.f+ic*0.55f)+0.5f);
    return d;
}

void render(const Image& src,const Image& dst,const Params& p,const FrameCtx& ctx){
    const int W=src.w,H=src.h;
    if(W<=0||H<=0) return;
    const float intensity=clampf((float)(p.intensity/100.0));
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
                float coverage=smoothstep(0.16f,0.84f,thresholded);
                // Let the paper break through the ink according to opacity.
                coverage *= (1.f - grainVisibility*0.34f*baseGrain);
                // Normal subtractive plate: ink replaces paper in covered areas.
                float a=clampf(coverage*intensity);
                accum.r=accum.r*(1.f-a)+ip.color.r*a;
                accum.g=accum.g*(1.f-a)+ip.color.g*a;
                accum.b=accum.b*(1.f-a)+ip.color.b*a;
            }
            float srcA=src.at(x,y)[3];
            out[0]=clampf(accum.r); out[1]=clampf(accum.g); out[2]=clampf(accum.b); out[3]=srcA;
        }
    }
}

} // namespace resoprint
