#ifndef RISOPRINT_CORE_H
#define RISOPRINT_CORE_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace resoprint {

struct Color { float r=1.f,g=1.f,b=1.f; };
struct Image { int w=0,h=0; float* px=nullptr; inline float* at(int x,int y) const { x=std::max(0,std::min(w-1,x)); y=std::max(0,std::min(h-1,y)); return px+((size_t)y*w+x)*4; } };

struct InkParams {
    bool enabled=true;
    Color color{1.f,1.f,1.f};
    double separation=0.5; // 0..1
    double shiftX=0.0;     // normalized image width
    double shiftY=0.0;     // normalized image height
};

struct Params {
    double intensity=100.0;
    int blend=1; // 1=Normal
    bool showSource=false;
    Color paper{1.f,1.f,1.f};
    double inkContrast=-0.33;
    double scale=0.21;
    double opacity=0.65;
    double softness=0.50;
    double contrast=1.18;
    double blendOriginal=0.0;
    InkParams ink[3];
};

struct FrameCtx { double timeSec=0.0; };

inline float clampf(float v,float a=0.f,float b=1.f){ return std::max(a,std::min(b,v)); }
inline float smoothstep(float a,float b,float x){ float t=clampf((x-a)/(b-a)); return t*t*(3.f-2.f*t); }
inline float luma(float r,float g,float b){ return 0.2126f*r+0.7152f*g+0.0722f*b; }
inline uint32_t hash_u32(uint32_t x){ x^=x>>16; x*=0x7feb352du; x^=x>>15; x*=0x846ca68bu; x^=x>>16; return x; }
inline uint32_t hash2(uint32_t a,uint32_t b){ return hash_u32(a ^ hash_u32(b+0x9e3779b9u)); }
inline float noise01(int x,int y,int seed=0){ uint32_t n=(uint32_t)x*374761393u ^ (uint32_t)y*668265263u ^ (uint32_t)seed*2246822519u; n=hash_u32(n); return (n & 0x00ffffffu)/16777216.f; }

void render(const Image& src,const Image& dst,const Params& p,const FrameCtx& ctx);

} // namespace resoprint
#endif
