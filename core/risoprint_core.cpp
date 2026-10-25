#include "risoprint_core.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

namespace resoprint {

// ---------------------------------------------------------------------------
// Spot Inks Catalog (Extracted from After Effects Screenshots & Strings)
// ---------------------------------------------------------------------------

const SpotInkDef SPOT_INKS_CATALOG[] = {
    {"Custom",             {0.314f, 0.851f, 0.945f}}, // Default Cyan #50D9F1
    {"Black",              {0.137f, 0.094f, 0.082f}}, // #231815
    {"Blue",               {0.000f, 0.471f, 0.749f}}, // #0078BF
    {"Bright Red",         {0.965f, 0.220f, 0.098f}}, // #F63819
    {"Brown",              {0.541f, 0.353f, 0.169f}}, // #8A5A2B
    {"Burgundy",           {0.573f, 0.125f, 0.247f}}, // #92203F
    {"Crimson",            {0.878f, 0.106f, 0.141f}}, // #E01B24
    {"Federal Blue",       {0.259f, 0.337f, 0.588f}}, // #425696
    {"Flat Gold",          {0.784f, 0.647f, 0.294f}}, // #C8A54B
    {"Fluorescent Green",  {0.000f, 1.000f, 0.400f}}, // #00FF66
    {"Fluorescent Orange", {1.000f, 0.451f, 0.000f}}, // #FF7300
    {"Fluorescent Pink",   {1.000f, 0.282f, 0.655f}}, // #FF48A7
    {"Fluorescent Red",    {1.000f, 0.200f, 0.200f}}, // #FF3333
    {"Fluorescent Yellow", {0.980f, 1.000f, 0.000f}}, // #FAFF00
    {"Forest",             {0.180f, 0.341f, 0.220f}}, // #2E5738
    {"Granite",            {0.451f, 0.451f, 0.451f}}, // #737373
    {"Green",              {0.000f, 0.627f, 0.380f}}, // #00A061
    {"Hunter Green",       {0.251f, 0.380f, 0.282f}}, // #406148
    {"Indigo",             {0.282f, 0.341f, 0.471f}}, // #485778
    {"Ivy",                {0.086f, 0.459f, 0.361f}}, // #16755C
    {"Lake",               {0.137f, 0.490f, 0.639f}}, // #237DA3
    {"Light Gray",         {0.706f, 0.706f, 0.706f}}, // #B4B4B4
    {"Light Lime",         {0.878f, 0.941f, 0.294f}}, // #E0F04B
    {"Light Mauve",        {0.729f, 0.627f, 0.729f}}, // #BAA0BA
    {"Light Teal",         {0.000f, 0.690f, 0.671f}}, // #00B0AB
    {"Marine Red",         {0.867f, 0.165f, 0.220f}}, // #DD2A38
    {"Medium Blue",        {0.196f, 0.400f, 0.690f}}, // #3266B0
    {"Melon",              {1.000f, 0.439f, 0.263f}}, // #FF7043
    {"Metallic Gold",      {0.671f, 0.541f, 0.251f}}, // #AB8A40
    {"Midnight",           {0.200f, 0.259f, 0.361f}}, // #33425C
    {"Mint",               {0.510f, 0.824f, 0.647f}}, // #82D2A5
    {"Orange",             {1.000f, 0.431f, 0.000f}}, // #FF6E00
    {"Orchid",             {0.749f, 0.329f, 0.588f}}, // #BF5496
    {"Paprika",            {0.933f, 0.314f, 0.200f}}, // #EE5033
    {"Pine",               {0.137f, 0.314f, 0.239f}}, // #23503D
    {"Plum",               {0.541f, 0.200f, 0.380f}}, // #8A3361
    {"Pumpkin",            {1.000f, 0.435f, 0.000f}}, // #FF6F00
    {"Purple",             {0.439f, 0.200f, 0.541f}}, // #70338A
    {"Raisin",             {0.451f, 0.220f, 0.294f}}, // #73384B
    {"Raspberry",          {0.812f, 0.063f, 0.365f}}, // #CF105D
    {"Red",                {1.000f, 0.220f, 0.157f}}, // #FF3828
    {"Scarlet",            {0.941f, 0.125f, 0.188f}}, // #F02030
    {"Sea Blue",           {0.000f, 0.408f, 0.639f}}, // #0068A3
    {"Seafoam",            {0.380f, 0.749f, 0.678f}}, // #61BFAD
    {"Sky Blue",           {0.294f, 0.690f, 0.878f}}, // #4BB0E0
    {"Slate",              {0.380f, 0.431f, 0.471f}}, // #616E78
    {"Smoky Teal",         {0.369f, 0.525f, 0.529f}}, // #5E8687
    {"Spruce",             {0.278f, 0.420f, 0.380f}}, // #476B61
    {"Steel",              {0.294f, 0.459f, 0.569f}}, // #4B7591
    {"Sunflower",          {1.000f, 0.769f, 0.000f}}, // #FFC400
    {"Sunshine",           {1.000f, 0.863f, 0.000f}}, // #FFDC00
    {"Teal",               {0.000f, 0.514f, 0.541f}}, // #00838A
    {"Turquoise",          {0.000f, 0.655f, 0.722f}}, // #00A7B8
    {"Violet",             {0.627f, 0.259f, 0.659f}}, // #A042A8
    {"Wine",               {0.569f, 0.188f, 0.251f}}, // #913040
    {"Yellow",             {1.000f, 0.914f, 0.000f}}  // #FFE900
};

const int SPOT_INKS_COUNT = sizeof(SPOT_INKS_CATALOG) / sizeof(SPOT_INKS_CATALOG[0]);

ColorRGB GetSpotInkRgbByIndex(int index) {
    if (index >= 1 && index <= SPOT_INKS_COUNT) {
        return SPOT_INKS_CATALOG[index - 1].rgb;
    }
    return SPOT_INKS_CATALOG[0].rgb;
}

// ---------------------------------------------------------------------------
// Math & Color Space Implementations
// ---------------------------------------------------------------------------

Vec3 RGBToHSV(ColorRGB rgb) {
    float r = Clamp01(rgb.r), g = Clamp01(rgb.g), b = Clamp01(rgb.b);
    float mx = std::max(r, std::max(g, b));
    float mn = std::min(r, std::min(g, b));
    float d = mx - mn;
    float h = 0.0f;
    if (d > 1e-5f) {
        if (mx == r) {
            h = std::fmod((g - b) / d, 6.0f);
        } else if (mx == g) {
            h = (b - r) / d + 2.0f;
        } else {
            h = (r - g) / d + 4.0f;
        }
        h /= 6.0f;
        if (h < 0.0f) h += 1.0f;
    }
    float s = (mx > 1e-5f) ? (d / mx) : 0.0f;
    float v = mx;
    return { h, s, v };
}

ColorRGB HSVToRGB(Vec3 hsv) {
    float h = hsv.x - std::floor(hsv.x);
    float s = Clamp01(hsv.y);
    float v = Clamp01(hsv.z);

    float fx = std::fabs(h * 6.0f - 3.0f);
    float fy = std::fabs(std::fmod(h + 4.0f / 6.0f, 1.0f) * 6.0f - 3.0f);
    float fz = std::fabs(std::fmod(h + 2.0f / 6.0f, 1.0f) * 6.0f - 3.0f);

    fx = Clamp01(fx - 1.0f);
    fy = Clamp01(fy - 1.0f);
    fz = Clamp01(fz - 1.0f);

    return {
        v * (1.0f + (fx - 1.0f) * s),
        v * (1.0f + (fy - 1.0f) * s),
        v * (1.0f + (fz - 1.0f) * s)
    };
}

// Procedural Hashes and Noise (Strictly Reconstructed from .rzdb Strings)
float RisoHash(Vec2 p) {
    float dotResult = p.x * 127.1f + p.y * 311.7f;
    float s = std::sin(dotResult) * 43758.5453123f;
    return s - std::floor(s);
}

Vec2 RisoHash2(Vec2 p) {
    float rx = std::sin(p.x * 127.1f + p.y * 311.7f) * 43758.5453f;
    float ry = std::sin(p.x * 269.5f + p.y * 183.3f) * 43758.5453f;
    return { rx - std::floor(rx), ry - std::floor(ry) };
}

float RisoHash3(Vec3 p) {
    float dotResult = p.x * 127.1f + p.y * 311.7f + p.z * 74.7f;
    float s = std::sin(dotResult) * 43758.5453123f;
    return s - std::floor(s);
}

float RisoNoise2D(Vec2 p) {
    Vec2 i{ std::floor(p.x), std::floor(p.y) };
    Vec2 f{ p.x - i.x, p.y - i.y };
    Vec2 u{ f.x * f.x * (3.0f - 2.0f * f.x), f.y * f.y * (3.0f - 2.0f * f.y) };

    float a = RisoHash(i);
    float b = RisoHash({ i.x + 1.0f, i.y });
    float c = RisoHash({ i.x, i.y + 1.0f });
    float d = RisoHash({ i.x + 1.0f, i.y + 1.0f });

    return Mix(Mix(a, b, u.x), Mix(c, d, u.x), u.y);
}

float RisoNoise3D(Vec3 p) {
    Vec3 i{ std::floor(p.x), std::floor(p.y), std::floor(p.z) };
    Vec3 f{ p.x - i.x, p.y - i.y, p.z - i.z };
    Vec3 u{
        f.x * f.x * (3.0f - 2.0f * f.x),
        f.y * f.y * (3.0f - 2.0f * f.y),
        f.z * f.z * (3.0f - 2.0f * f.z)
    };

    float n000 = RisoHash3(i);
    float n100 = RisoHash3({ i.x + 1.0f, i.y, i.z });
    float n010 = RisoHash3({ i.x, i.y + 1.0f, i.z });
    float n110 = RisoHash3({ i.x + 1.0f, i.y + 1.0f, i.z });
    float n001 = RisoHash3({ i.x, i.y, i.z + 1.0f });
    float n101 = RisoHash3({ i.x + 1.0f, i.y, i.z + 1.0f });
    float n011 = RisoHash3({ i.x, i.y + 1.0f, i.z + 1.0f });
    float n111 = RisoHash3({ i.x + 1.0f, i.y + 1.0f, i.z + 1.0f });

    float nx00 = Mix(n000, n100, u.x);
    float nx10 = Mix(n010, n110, u.x);
    float nx01 = Mix(n001, n101, u.x);
    float nx11 = Mix(n011, n111, u.x);

    float nxy0 = Mix(nx00, nx10, u.y);
    float nxy1 = Mix(nx01, nx11, u.y);

    return Mix(nxy0, nxy1, u.z);
}

float RisoFBM(Vec2 p, int octaves) {
    float val = 0.0f;
    float amp = 0.5f;
    Vec2 q = p;
    for (int k = 0; k < octaves; ++k) {
        val += amp * RisoNoise2D(q);
        q.x *= 2.0f;
        q.y *= 2.0f;
        amp *= 0.5f;
    }
    return val;
}

float RisoFBM3D(Vec3 p, int octaves) {
    float val = 0.0f;
    float amp = 0.5f;
    Vec3 q = p;
    for (int k = 0; k < octaves; ++k) {
        val += amp * RisoNoise3D(q);
        q.x *= 2.0f;
        q.y *= 2.0f;
        q.z *= 2.0f;
        amp *= 0.5f;
    }
    return val;
}

// ---------------------------------------------------------------------------
// Halftone Geometry & Screen Evaluators
// ---------------------------------------------------------------------------

float RisoCellDist(Vec2 d, int shape) {
    if (shape == 2) { // Ellipse
        float dx = d.x;
        float dy = d.y * 1.5f;
        return std::sqrt(dx * dx + dy * dy);
    } else if (shape == 3) { // Diamond
        return std::fabs(d.x) + std::fabs(d.y);
    } else if (shape == 4) { // Square
        return std::max(std::fabs(d.x), std::fabs(d.y));
    }
    // Circle
    return std::sqrt(d.x * d.x + d.y * d.y);
}

void RisoVoronoiCell(Vec2 p, int shape, float randomness, float* outDist, Vec2* outCenter) {
    Vec2 ip{ std::floor(p.x), std::floor(p.y) };
    float hardMin = 10.0f;
    Vec2 bestCenter{ 0.0f, 0.0f };

    for (int j = -1; j <= 1; ++j) {
        for (int i = -1; i <= 1; ++i) {
            Vec2 neighbor{ ip.x + (float)i, ip.y + (float)j };
            Vec2 rndOffset = RisoHash2(neighbor);
            Vec2 center{
                neighbor.x + Mix(0.5f, rndOffset.x, randomness),
                neighbor.y + Mix(0.5f, rndOffset.y, randomness)
            };
            Vec2 d{ p.x - center.x, p.y - center.y };
            float dist = RisoCellDist(d, shape);
            if (dist < hardMin) {
                hardMin = dist;
                bestCenter = center;
            }
        }
    }
    if (outDist) *outDist = hardMin;
    if (outCenter) *outCenter = bestCenter;
}

float RisoApplyThreshold(float dist, float channelVal, float factor, float intensity, float smoothness) {
    float adjusted = Clamp01(channelVal * factor);
    float sat = Smoothstep(0.7f, 1.0f, adjusted);
    float effI = intensity * Mix(1.0f, 0.55f, sat);

    float len = dist * effI * 1.7320508f;
    float r = std::max(1.0f - len, 0.0f);
    float gradSq = r * r;
    float inverted = 1.0f - gradSq;

    float swidth = smoothness * smoothness * 0.4f;
    float th = (swidth < 1e-4f)
        ? (adjusted >= inverted ? 1.0f : 0.0f)
        : Smoothstep(inverted - swidth, inverted + swidth, adjusted);

    float saturationFloor = Smoothstep(0.92f, 0.99f, adjusted);
    return Mix(th, 1.0f, saturationFloor);
}

float RisoUnionDotInk(Vec2 gridPos, int shape, float randomness, float channelVal, float factor, float intensity, float smoothness) {
    Vec2 ip{ std::floor(gridPos.x), std::floor(gridPos.y) };
    float ink = 0.0f;

    for (int j = -1; j <= 1; ++j) {
        for (int i = -1; i <= 1; ++i) {
            Vec2 neighbor{ ip.x + (float)i, ip.y + (float)j };
            Vec2 rndOffset = RisoHash2(neighbor);
            Vec2 center{
                neighbor.x + Mix(0.5f, rndOffset.x, randomness),
                neighbor.y + Mix(0.5f, rndOffset.y, randomness)
            };
            Vec2 d{ gridPos.x - center.x, gridPos.y - center.y };
            float dist = RisoCellDist(d, shape);
            float v = RisoApplyThreshold(dist, channelVal, factor, intensity, smoothness);
            if (v > ink) ink = v;
        }
    }
    return ink;
}

static const float BAYER_8X8[8][8] = {
    {  0.0f/64.0f, 32.0f/64.0f,  8.0f/64.0f, 40.0f/64.0f,  2.0f/64.0f, 34.0f/64.0f, 10.0f/64.0f, 42.0f/64.0f },
    { 48.0f/64.0f, 16.0f/64.0f, 56.0f/64.0f, 24.0f/64.0f, 50.0f/64.0f, 18.0f/64.0f, 58.0f/64.0f, 26.0f/64.0f },
    { 12.0f/64.0f, 44.0f/64.0f,  4.0f/64.0f, 36.0f/64.0f, 14.0f/64.0f, 46.0f/64.0f,  6.0f/64.0f, 38.0f/64.0f },
    { 60.0f/64.0f, 28.0f/64.0f, 52.0f/64.0f, 20.0f/64.0f, 62.0f/64.0f, 30.0f/64.0f, 54.0f/64.0f, 22.0f/64.0f },
    {  3.0f/64.0f, 35.0f/64.0f, 11.0f/64.0f, 43.0f/64.0f,  1.0f/64.0f, 33.0f/64.0f,  9.0f/64.0f, 41.0f/64.0f },
    { 51.0f/64.0f, 19.0f/64.0f, 59.0f/64.0f, 27.0f/64.0f, 49.0f/64.0f, 17.0f/64.0f, 57.0f/64.0f, 25.0f/64.0f },
    { 15.0f/64.0f, 47.0f/64.0f,  7.0f/64.0f, 39.0f/64.0f, 13.0f/64.0f, 45.0f/64.0f,  5.0f/64.0f, 37.0f/64.0f },
    { 63.0f/64.0f, 31.0f/64.0f, 55.0f/64.0f, 23.0f/64.0f, 61.0f/64.0f, 29.0f/64.0f, 53.0f/64.0f, 21.0f/64.0f }
};

float RisoBayer8Value(int ix, int iy) {
    int x = (ix % 8 + 8) % 8;
    int y = (iy % 8 + 8) % 8;
    return BAYER_8X8[y][x];
}

float RisoPseudoBlueNoise(Vec2 p) {
    float h1 = RisoHash(p);
    float h2 = RisoHash({ p.y * 1.618f, p.x * 1.618f });
    float h3 = RisoHash({ p.x + 37.1f, p.y + 91.3f });
    float v = (h1 + h2 + h3) * 0.333333f;
    // High-pass filter modulation
    return Smoothstep(0.15f, 0.85f, v);
}

// ---------------------------------------------------------------------------
// Coordinate Transforms
// ---------------------------------------------------------------------------

Vec2 RisoApplyPlateXform(Vec2 uv, float plateRotRad, float plateSkew, float resW, float resH) {
    if (plateRotRad == 0.0f && plateSkew == 0.0f) return uv;
    Vec2 q;
    q.x = (uv.x - 0.5f) * resW;
    q.y = (uv.y - 0.5f) * resH;
    q.x += q.y * plateSkew;
    float c = std::cos(plateRotRad), s = std::sin(plateRotRad);
    float qx = q.x * c - q.y * s;
    float qy = q.x * s + q.y * c;
    return { qx / resW + 0.5f, qy / resH + 0.5f };
}

Vec2 RisoApplyPlateXformInverse(Vec2 uv, float plateRotRad, float plateSkew, float resW, float resH) {
    if (plateRotRad == 0.0f && plateSkew == 0.0f) return uv;
    Vec2 q;
    q.x = (uv.x - 0.5f) * resW;
    q.y = (uv.y - 0.5f) * resH;
    float c = std::cos(-plateRotRad), s = std::sin(-plateRotRad);
    float qx = q.x * c - q.y * s;
    float qy = q.x * s + q.y * c;
    qx -= qy * plateSkew;
    return { qx / resW + 0.5f, qy / resH + 0.5f };
}

Vec2 RisoComputeGridPos(Vec2 uv, float plateRotRad, float plateSkew, float angleRad, float offX, float offY, float freq, float aspect, float resW, float resH) {
    Vec2 pUV = RisoApplyPlateXform(uv, plateRotRad, plateSkew, resW, resH);
    float px = (pUV.x - 0.5f) * resW + offX;
    float py = (pUV.y - 0.5f) * resH + offY;

    float ca = std::cos(angleRad), sa = std::sin(angleRad);
    float rx = px * ca - py * sa;
    float ry = px * sa + py * ca;

    float scale = freq / std::max(resH, 1.0f);
    return { rx * scale, ry * scale * std::max(aspect, 0.01f) };
}

Vec2 RisoGridToUV(Vec2 gridPos, float plateRotRad, float plateSkew, float angleRad, float offX, float offY, float freq, float aspect, float resW, float resH) {
    float scale = freq / std::max(resH, 1.0f);
    if (scale < 1e-6f) return { 0.5f, 0.5f };

    float rx = gridPos.x / scale;
    float ry = gridPos.y / (scale * std::max(aspect, 0.01f));

    float ca = std::cos(-angleRad), sa = std::sin(-angleRad);
    float px = rx * ca - ry * sa - offX;
    float py = rx * sa + ry * ca - offY;

    Vec2 uv{ px / resW + 0.5f, py / resH + 0.5f };
    return RisoApplyPlateXformInverse(uv, plateRotRad, plateSkew, resW, resH);
}

// ---------------------------------------------------------------------------
// Line Art Mask Extraction
// ---------------------------------------------------------------------------

float RisoLineArtMask(float centerLuma, float edgeContrast, float threshold01, float strength01) {
    threshold01 = Clamp01(threshold01);
    strength01 = Clamp01(strength01);
    float darkMask = Clamp01((threshold01 - centerLuma) * 4.0f);
    float edgeStart = 0.18f - threshold01 * 0.10f;
    float edgeMask = Clamp01((edgeContrast - edgeStart) * 5.0f);
    return Clamp01(std::max(darkMask, edgeMask) * strength01);
}

// ---------------------------------------------------------------------------
// Spot Color Separation (Greedy & Balanced Mix)
// ---------------------------------------------------------------------------

static inline float Dot3(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec4 RisoSpotGreedyDecompose(Vec3 target, Vec3 a0, Vec3 a1, Vec3 a2, Vec3 a3, int spotPlateCount) {
    float d0 = Clamp01(Dot3(a0, target) / std::max(Dot3(a0, a0), 1e-3f));
    target.x = std::max(0.0f, target.x - d0 * a0.x);
    target.y = std::max(0.0f, target.y - d0 * a0.y);
    target.z = std::max(0.0f, target.z - d0 * a0.z);

    float d1 = Clamp01(Dot3(a1, target) / std::max(Dot3(a1, a1), 1e-3f));
    target.x = std::max(0.0f, target.x - d1 * a1.x);
    target.y = std::max(0.0f, target.y - d1 * a1.y);
    target.z = std::max(0.0f, target.z - d1 * a1.z);

    float d2 = 0.0f;
    if (spotPlateCount >= 3) {
        d2 = Clamp01(Dot3(a2, target) / std::max(Dot3(a2, a2), 1e-3f));
        target.x = std::max(0.0f, target.x - d2 * a2.x);
        target.y = std::max(0.0f, target.y - d2 * a2.y);
        target.z = std::max(0.0f, target.z - d2 * a2.z);
    }

    float d3 = 0.0f;
    if (spotPlateCount >= 4) {
        d3 = Clamp01(Dot3(a3, target) / std::max(Dot3(a3, a3), 1e-3f));
    }

    return { d0, d1, d2, d3 };
}

static inline float RisoSpotCoordUpdate(Vec3 target, Vec3 a_i, float d_i,
                                       Vec3 a0, float d0, Vec3 a1, float d1,
                                       Vec3 a2, float d2, Vec3 a3, float d3) {
    Vec3 r = target;
    r.x -= (a0.x * d0 + a1.x * d1 + a2.x * d2 + a3.x * d3 - a_i.x * d_i);
    r.y -= (a0.y * d0 + a1.y * d1 + a2.y * d2 + a3.y * d3 - a_i.y * d_i);
    r.z -= (a0.z * d0 + a1.z * d1 + a2.z * d2 + a3.z * d3 - a_i.z * d_i);
    float denom = std::max(Dot3(a_i, a_i), 1e-3f);
    return Clamp01(Dot3(a_i, r) / denom);
}

Vec4 RisoSpotBalancedDecompose(Vec3 target, Vec3 a0, Vec3 a1, Vec3 a2, Vec3 a3, int spotPlateCount) {
    float d0 = 0.0f, d1 = 0.0f, d2 = 0.0f, d3 = 0.0f;

    for (int sweep = 0; sweep < 4; ++sweep) {
        d0 = RisoSpotCoordUpdate(target, a0, d0, a0, d0, a1, d1, a2, d2, a3, d3);
        d1 = RisoSpotCoordUpdate(target, a1, d1, a0, d0, a1, d1, a2, d2, a3, d3);
        if (spotPlateCount >= 3) {
            d2 = RisoSpotCoordUpdate(target, a2, d2, a0, d0, a1, d1, a2, d2, a3, d3);
        }
        if (spotPlateCount >= 4) {
            d3 = RisoSpotCoordUpdate(target, a3, d3, a0, d0, a1, d1, a2, d2, a3, d3);
        }
        if (spotPlateCount >= 3) {
            d2 = RisoSpotCoordUpdate(target, a2, d2, a0, d0, a1, d1, a2, d2, a3, d3);
        }
        d1 = RisoSpotCoordUpdate(target, a1, d1, a0, d0, a1, d1, a2, d2, a3, d3);
        d0 = RisoSpotCoordUpdate(target, a0, d0, a0, d0, a1, d1, a2, d2, a3, d3);
    }

    return {
        d0,
        d1,
        (spotPlateCount >= 3) ? d2 : 0.0f,
        (spotPlateCount >= 4) ? d3 : 0.0f
    };
}

// ---------------------------------------------------------------------------
// Presets
// ---------------------------------------------------------------------------

void ApplyPressPreset(int presetIdx, PluginParams& p) {
    if (presetIdx <= 1) return; // Custom
    switch (presetIdx) {
        case 2: // Soft Studio
            p.screenType = 1; p.screenDensity = 220.0f; p.softness = 30.0f;
            p.paperGrain = 15.0f; p.dotGain = 1.05f; p.overprintMode = 2;
            break;
        case 3: // 1970s Zine
            p.screenType = 2; p.screenDensity = 160.0f; p.distortion = 25.0f;
            p.paperType = 4; p.paperGrain = 35.0f; p.inkFlecks = 20.0f;
            break;
        case 4: // Blown-out Punk
            p.screenType = 1; p.screenDensity = 140.0f; p.dotHardness = 180.0f;
            p.contrast = 35.0f; p.brightness = 10.0f; p.lineArtStrength = 80.0f;
            break;
        case 5: // Newsprint
            p.screenType = 4; p.screenDensity = 130.0f; p.paperType = 2;
            p.dotGain = 1.25f; p.agingAmount = 30.0f;
            break;
        case 6: // RisoFest 2-color
            p.colorMode = 2; p.activePlates = 2; p.spotSeparation = 2;
            p.screenType = 1; p.screenDensity = 190.0f;
            break;
        case 7: // Letterpress Indigo
            p.colorMode = 2; p.activePlates = 2; p.screenType = 5;
            p.paperGrain = 28.0f; p.inkPooling = 15.0f;
            break;
        case 8: // Bright Comic
            p.screenType = 1; p.dotShape = 1; p.screenDensity = 200.0f;
            p.saturation = 20.0f; p.blackBoost = 130.0f;
            break;
        default:
            p.screenDensity = 200.0f;
            break;
    }
}

void ApplyMachineProfile(int profileIdx, PluginParams& p) {
    if (profileIdx <= 1) return;
    switch (profileIdx) {
        case 2: // GR3750 (400 DPI)
            p.screenDensity = 180.0f; p.inkVariance = 20.0f; p.dotGain = 1.15f; break;
        case 3: // EZ591 (600e, single-drum)
            p.screenDensity = 240.0f; p.inkVariance = 12.0f; break;
        case 4: // MZ-790 (600e, 2-color)
            p.screenDensity = 250.0f; p.cyanOffsetX = 1.5f; p.magentaOffsetX = -1.2f; break;
        case 5: // RZ970 (600x600)
            p.screenDensity = 260.0f; p.pressFidelity = 15.0f; break;
        case 6: // SF5350 (300x600)
            p.screenDensity = 190.0f; p.dotStretch = 15.0f; break;
        case 7: // CR1610 (Compact 300 DPI)
            p.screenDensity = 150.0f; p.inkVariance = 25.0f; break;
        default: break;
    }
}

void ApplySpotKit(int kitIdx, PluginParams& p) {
    if (kitIdx <= 1) return;
    p.colorMode = 2; // Switch to Spot Riso
    switch (kitIdx) {
        case 2: // Risograph Pink+Blue
            p.activePlates = 2;
            p.customSpotInks[0] = {1.000f, 0.282f, 0.655f}; // Fluorescent Pink
            p.customSpotInks[1] = {0.000f, 0.471f, 0.749f}; // Blue
            break;
        case 3: // Cool Trio (Mint+Sky+Teal)
            p.activePlates = 3;
            p.customSpotInks[0] = {0.510f, 0.824f, 0.647f}; // Mint
            p.customSpotInks[1] = {0.294f, 0.690f, 0.878f}; // Sky Blue
            p.customSpotInks[2] = {0.000f, 0.514f, 0.541f}; // Teal
            break;
        case 4: // Earthy (Brown+Burgundy+Gold)
            p.activePlates = 3;
            p.customSpotInks[0] = {0.541f, 0.353f, 0.169f}; // Brown
            p.customSpotInks[1] = {0.573f, 0.125f, 0.247f}; // Burgundy
            p.customSpotInks[2] = {0.784f, 0.647f, 0.294f}; // Flat Gold
            break;
        case 5: // Twilight (Indigo+Plum+Crimson)
            p.activePlates = 3;
            p.customSpotInks[0] = {0.282f, 0.341f, 0.471f}; // Indigo
            p.customSpotInks[1] = {0.541f, 0.200f, 0.380f}; // Plum
            p.customSpotInks[2] = {0.878f, 0.106f, 0.141f}; // Crimson
            break;
        case 6: // Sunset (Sunflower+Pumpkin+Red)
            p.activePlates = 3;
            p.customSpotInks[0] = {1.000f, 0.769f, 0.000f}; // Sunflower
            p.customSpotInks[1] = {1.000f, 0.435f, 0.000f}; // Pumpkin
            p.customSpotInks[2] = {1.000f, 0.220f, 0.157f}; // Red
            break;
        case 7: // Forest (Hunter+Sea+Brown)
            p.activePlates = 3;
            p.customSpotInks[0] = {0.251f, 0.380f, 0.282f}; // Hunter Green
            p.customSpotInks[1] = {0.000f, 0.408f, 0.639f}; // Sea Blue
            p.customSpotInks[2] = {0.541f, 0.353f, 0.169f}; // Brown
            break;
        case 8: // Mint+Pink Duo
            p.activePlates = 2;
            p.customSpotInks[0] = {0.510f, 0.824f, 0.647f}; // Mint
            p.customSpotInks[1] = {1.000f, 0.282f, 0.655f}; // Fluorescent Pink
            break;
        case 9: // Cyan+Magenta Duo
            p.activePlates = 2;
            p.customSpotInks[0] = {0.000f, 0.627f, 0.914f}; // Cyan
            p.customSpotInks[1] = {0.894f, 0.0f, 0.498f};   // Magenta
            break;
        default: break;
    }
}

// ---------------------------------------------------------------------------
// Bilinear / Soft Sampling Helper
// ---------------------------------------------------------------------------

static inline ColorRGBA SampleBilinear(const ImageBuffer& im, float fx, float fy) {
    fx = std::max(0.0f, std::min((float)(im.w - 1), fx));
    fy = std::max(0.0f, std::min((float)(im.h - 1), fy));
    int x0 = (int)std::floor(fx);
    int y0 = (int)std::floor(fy);
    int x1 = std::min(im.w - 1, x0 + 1);
    int y1 = std::min(im.h - 1, y0 + 1);
    float tx = fx - (float)x0;
    float ty = fy - (float)y0;

    const float* a = im.at_const(x0, y0);
    const float* b = im.at_const(x1, y0);
    const float* c = im.at_const(x0, y1);
    const float* d = im.at_const(x1, y1);

    ColorRGBA o;
    for (int k = 0; k < 4; ++k) {
        float ab = a[k] + (b[k] - a[k]) * tx;
        float cd = c[k] + (d[k] - c[k]) * tx;
        (&o.r)[k] = ab + (cd - ab) * ty;
    }
    return o;
}

// ---------------------------------------------------------------------------
// Main Rendering Engine Implementation
// ---------------------------------------------------------------------------

void RenderRisoprint(
    const ImageBuffer& src,
    ImageBuffer& dst,
    const PluginParams& inParams,
    double frameTimeSec
) {
    if (src.w <= 0 || src.h <= 0 || !src.data || !dst.data) return;

    PluginParams p = inParams;
    ApplyPressPreset(p.pressPreset, p);
    ApplyMachineProfile(p.machineProfile, p);
    ApplySpotKit(p.spotInkKit, p);

    const int W = std::min(src.w, dst.w);
    const int H = std::min(src.h, dst.h);
    const float fW = (float)W;
    const float fH = (float)H;

    // Resolve Spot Inks if in Spot Mode
    ColorRGB spotInks[4];
    for (int i = 0; i < 4; ++i) {
        int idx = (i == 0) ? p.spotInk1 : (i == 1) ? p.spotInk2 : (i == 2) ? p.spotInk3 : p.spotInk4;
        if (idx > 1) {
            spotInks[i] = GetSpotInkRgbByIndex(idx);
        } else {
            spotInks[i] = p.customSpotInks[i];
        }
    }

    // Plate angles & offsets
    const float angles[4] = {
        p.angleCyan * 0.0174532925f,
        p.angleMagenta * 0.0174532925f,
        p.angleYellow * 0.0174532925f,
        p.angleBlack * 0.0174532925f
    };
    const float plateRot[4] = {
        (p.allRotation + p.cyanRotation) * 0.0174532925f,
        (p.allRotation + p.magentaRotation) * 0.0174532925f,
        (p.allRotation + p.yellowRotation) * 0.0174532925f,
        (p.allRotation + p.blackRotation) * 0.0174532925f
    };
    const float plateSkew[4] = {
        p.allSkew + p.cyanSkew,
        p.allSkew + p.magentaSkew,
        p.allSkew + p.yellowSkew,
        p.allSkew + p.blackSkew
    };
    const float offX[4] = {
        p.allOffsetX + p.cyanOffsetX,
        p.allOffsetX + p.magentaOffsetX,
        p.allOffsetX + p.yellowOffsetX,
        p.allOffsetX + p.blackOffsetX
    };
    const float offY[4] = {
        p.allOffsetY + p.cyanOffsetY,
        p.allOffsetY + p.magentaOffsetY,
        p.allOffsetY + p.yellowOffsetY,
        p.allOffsetY + p.blackOffsetY
    };
    const float plateWeights[4] = {
        p.weightCyan / 100.0f,
        p.weightMagenta / 100.0f,
        p.weightYellow / 100.0f,
        p.weightBlack / 100.0f
    };
    const float plateCurves[4] = {
        std::max(0.1f, p.cyanCurve),
        std::max(0.1f, p.magentaCurve),
        std::max(0.1f, p.yellowCurve),
        std::max(0.1f, p.blackCurve)
    };
    const bool plateEn[4] = {
        p.cyanPlateEn,
        p.magentaPlateEn,
        p.yellowPlateEn,
        p.blackPlateEn
    };

    const float drumAnimW = (float)frameTimeSec * p.drumAnimSpeed * 2.0f;
    const float halftoneAspect = 1.0f + p.dotStretch * 0.01f;
    const float edgeHalf = std::max(0.0f, std::min(0.10f, p.edgeSoftness * 0.001f));

    // Magnify params
    const float zoom = std::max(1.0f, p.magnifyZoom);
    const float magCenterX = p.magnifyCenterX * 0.01f;
    const float magCenterY = p.magnifyCenterY * 0.01f;

    // Main pixel rendering loop
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            Vec2 uv{ ((float)x + 0.5f) / fW, ((float)y + 0.5f) / fH };

            // Apply Magnify Zoom
            if (zoom > 1.001f) {
                uv.x = magCenterX + (uv.x - 0.5f) / zoom;
                uv.y = magCenterY + (uv.y - 0.5f) / zoom;
            }

            // Sample Source
            ColorRGBA srcSample = SampleBilinear(src, uv.x * fW, uv.y * fH);
            ColorRGB rgb{ srcSample.r, srcSample.g, srcSample.b };

            // Working Space Linearization
            if (p.workingSpace == 2) {
                rgb = SrgbToLinear3(rgb);
            }

            // ---------------------------------------------------------------
            // Pass 1: Color Adjustments & Separation
            // ---------------------------------------------------------------
            // Brightness / Contrast
            if (std::fabs(p.brightness) > 0.01f || std::fabs(p.contrast) > 0.01f) {
                float cFactor = 1.0f + p.contrast * 0.01f;
                float bOffset = p.brightness * 0.01f;
                rgb.r = Clamp01((rgb.r - 0.5f) * cFactor + 0.5f + bOffset);
                rgb.g = Clamp01((rgb.g - 0.5f) * cFactor + 0.5f + bOffset);
                rgb.b = Clamp01((rgb.b - 0.5f) * cFactor + 0.5f + bOffset);
            }

            // Hue / Saturation / HSV Value
            if (std::fabs(p.hue) > 0.01f || std::fabs(p.saturation) > 0.01f || std::fabs(p.hsvValue) > 0.01f) {
                Vec3 hsv = RGBToHSV(rgb);
                hsv.x = std::fmod(hsv.x + p.hue / 360.0f + 1.0f, 1.0f);
                hsv.y = Clamp01(hsv.y * (1.0f + p.saturation * 0.01f));
                hsv.z = Clamp01(hsv.z * (1.0f + p.hsvValue * 0.01f));
                rgb = HSVToRGB(hsv);
            }

            // Gamma
            if (std::fabs(p.gamma - 1.0f) > 0.01f) {
                float invGamma = 1.0f / std::max(0.01f, p.gamma);
                rgb.r = std::pow(Clamp01(rgb.r), invGamma);
                rgb.g = std::pow(Clamp01(rgb.g), invGamma);
                rgb.b = std::pow(Clamp01(rgb.b), invGamma);
            }

            // Line Art Edge Extraction
            float lineMask = 0.0f;
            if (p.lineArtPlate != 1 && p.lineArtStrength > 0.01f) {
                float centerLuma = Luma3(rgb);
                float lumaL = Luma3({ SampleBilinear(src, (uv.x - 1.0f / fW) * fW, uv.y * fH).r,
                                      SampleBilinear(src, (uv.x - 1.0f / fW) * fW, uv.y * fH).g,
                                      SampleBilinear(src, (uv.x - 1.0f / fW) * fW, uv.y * fH).b });
                float lumaR = Luma3({ SampleBilinear(src, (uv.x + 1.0f / fW) * fW, uv.y * fH).r,
                                      SampleBilinear(src, (uv.x + 1.0f / fW) * fW, uv.y * fH).g,
                                      SampleBilinear(src, (uv.x + 1.0f / fW) * fW, uv.y * fH).b });
                float lumaU = Luma3({ SampleBilinear(src, uv.x * fW, (uv.y - 1.0f / fH) * fH).r,
                                      SampleBilinear(src, uv.x * fW, (uv.y - 1.0f / fH) * fH).g,
                                      SampleBilinear(src, uv.x * fW, (uv.y - 1.0f / fH) * fH).b });
                float lumaD = Luma3({ SampleBilinear(src, uv.x * fW, (uv.y + 1.0f / fH) * fH).r,
                                      SampleBilinear(src, uv.x * fW, (uv.y + 1.0f / fH) * fH).g,
                                      SampleBilinear(src, uv.x * fW, (uv.y + 1.0f / fH) * fH).b });
                float edgeContrast = std::max(std::fabs(centerLuma - lumaL),
                                     std::max(std::fabs(centerLuma - lumaR),
                                     std::max(std::fabs(centerLuma - lumaU), std::fabs(centerLuma - lumaD))));
                lineMask = RisoLineArtMask(centerLuma, edgeContrast, p.lineArtThreshold * 0.01f, p.lineArtStrength * 0.01f);
            }

            // Plate Separation
            Vec4 sepCoverages{ 0.0f, 0.0f, 0.0f, 0.0f }; // C, M, Y, K or Spot 1..4
            if (p.colorMode == 2) {
                // Spot Riso
                Vec3 target{ 1.0f - rgb.r, 1.0f - rgb.g, 1.0f - rgb.b };
                Vec3 a0{ 1.0f - spotInks[0].r, 1.0f - spotInks[0].g, 1.0f - spotInks[0].b };
                Vec3 a1{ 1.0f - spotInks[1].r, 1.0f - spotInks[1].g, 1.0f - spotInks[1].b };
                Vec3 a2{ 1.0f - spotInks[2].r, 1.0f - spotInks[2].g, 1.0f - spotInks[2].b };
                Vec3 a3{ 1.0f - spotInks[3].r, 1.0f - spotInks[3].g, 1.0f - spotInks[3].b };

                if (p.spotSeparation == 2) {
                    sepCoverages = RisoSpotBalancedDecompose(target, a0, a1, a2, a3, p.activePlates);
                } else {
                    sepCoverages = RisoSpotGreedyDecompose(target, a0, a1, a2, a3, p.activePlates);
                }
            } else {
                // CMYK
                float cmyR = 1.0f - rgb.r;
                float cmyG = 1.0f - rgb.g;
                float cmyB = 1.0f - rgb.b;
                float k = std::min(cmyR, std::min(cmyG, cmyB));

                float cmyNormR = 0.0f, cmyNormG = 0.0f, cmyNormB = 0.0f;
                if (k < 0.999f) {
                    float invKK = 1.0f / (1.0f - k);
                    cmyNormR = (cmyR - k) * invKK;
                    cmyNormG = (cmyG - k) * invKK;
                    cmyNormB = (cmyB - k) * invKK;
                }
                float enrich = p.richBlacks * 0.01f;
                sepCoverages.x = cmyNormR + (cmyR - cmyNormR) * enrich;
                sepCoverages.y = cmyNormG + (cmyG - cmyNormG) * enrich;
                sepCoverages.z = cmyNormB + (cmyB - cmyNormB) * enrich;
                sepCoverages.w = k * (1.0f - enrich) * (p.blackBoost * 0.01f);
            }

            // Apply Line Art to Target Channel
            if (lineMask > 0.0f && p.lineArtPlate > 1) {
                int targetCh = (p.lineArtPlate == 2) ? 3 : (p.lineArtPlate - 3);
                if (targetCh == 0) sepCoverages.x = std::max(sepCoverages.x, lineMask);
                else if (targetCh == 1) sepCoverages.y = std::max(sepCoverages.y, lineMask);
                else if (targetCh == 2) sepCoverages.z = std::max(sepCoverages.z, lineMask);
                else if (targetCh == 3) sepCoverages.w = std::max(sepCoverages.w, lineMask);
            }

            // ---------------------------------------------------------------
            // Pass 2: Halftone & Screen Processing
            // ---------------------------------------------------------------
            float chVals[4] = { sepCoverages.x, sepCoverages.y, sepCoverages.z, sepCoverages.w };
            float chDots[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

            // Specks Noise
            float specksAmount = p.inkFlecks * 0.01f;
            float specksScale = std::max(1.0f, p.fleckScale);

            for (int ch = 0; ch < 4; ++ch) {
                if (!plateEn[ch]) continue;

                // Specks noise
                float spk = 0.0f;
                if (specksAmount > 0.001f) {
                    Vec3 spkPos{ uv.x * specksScale + (float)ch * 13.7f, uv.y * specksScale + 7.1f, drumAnimW };
                    spk = RisoFBM3D(spkPos, 2) * specksAmount * 0.5f;
                }

                float rawVal = Clamp01((chVals[ch] - spk) * plateWeights[ch] * (p.coverage * 0.01f));

                // Screen frequency
                float freq = p.screenDensity;
                float cellPx = fH / std::max(freq, 1.0f);
                float bayerStep = std::max(cellPx / 8.0f, 1.0f);

                Vec2 perPlateUV = RisoApplyPlateXform(uv, plateRot[ch], plateSkew[ch], fW, fH);
                perPlateUV.x += offX[ch] / fW;
                perPlateUV.y += offY[ch] / fH;
                perPlateUV.y *= halftoneAspect;

                // Distortion
                if (p.distortion > 0.01f) {
                    float distScale = p.distortion * 0.01f * (cellPx / fH);
                    Vec2 hpx{ perPlateUV.x * fW + 7.1f + (float)ch * 4.3f, perPlateUV.y * fH + 13.7f };
                    Vec2 hpy{ perPlateUV.x * fW + 31.3f + (float)ch * 9.7f, perPlateUV.y * fH + 5.1f };
                    perPlateUV.x += (RisoHash(hpx) - 0.5f) * distScale;
                    perPlateUV.y += (RisoHash(hpy) - 0.5f) * distScale;
                }

                if (p.screenType == 2) {
                    // Bayer 8x8
                    int ix = (int)std::floor(perPlateUV.x * fW / bayerStep);
                    int iy = (int)std::floor(perPlateUV.y * fH / bayerStep);
                    float th = RisoBayer8Value(ix, iy);
                    chDots[ch] = (edgeHalf > 0.0001f)
                        ? Smoothstep((1.0f - rawVal) - edgeHalf, (1.0f - rawVal) + edgeHalf, th)
                        : (rawVal >= (1.0f - th) ? 1.0f : 0.0f);
                } else if (p.screenType == 3) {
                    // Blue noise
                    float th = RisoPseudoBlueNoise({ perPlateUV.x * fW / cellPx, perPlateUV.y * fH / cellPx });
                    chDots[ch] = (edgeHalf > 0.0001f)
                        ? Smoothstep((1.0f - rawVal) - edgeHalf, (1.0f - rawVal) + edgeHalf, th)
                        : (rawVal >= (1.0f - th) ? 1.0f : 0.0f);
                } else if (p.screenType == 4) {
                    // Line screen
                    float coord = perPlateUV.x * std::cos(angles[ch]) + perPlateUV.y * std::sin(angles[ch]);
                    float th = 0.5f + 0.5f * std::sin(coord * freq * 6.2831853f);
                    chDots[ch] = (edgeHalf > 0.0001f)
                        ? Smoothstep((1.0f - rawVal) - edgeHalf, (1.0f - rawVal) + edgeHalf, th)
                        : (rawVal >= (1.0f - th) ? 1.0f : 0.0f);
                } else if (p.screenType == 5) {
                    // Stippled
                    float th = RisoHash({ perPlateUV.x * fW * 0.75f, perPlateUV.y * fH * 0.75f });
                    chDots[ch] = (edgeHalf > 0.0001f)
                        ? Smoothstep((1.0f - rawVal) - edgeHalf, (1.0f - rawVal) + edgeHalf, th)
                        : (rawVal >= (1.0f - th) ? 1.0f : 0.0f);
                } else if (p.screenType == 6) {
                    // Atkinson sparse approximation
                    Vec2 ppos{ std::floor(perPlateUV.x * fW / cellPx), std::floor(perPlateUV.y * fH / cellPx) };
                    float h0 = RisoHash(ppos);
                    float h1 = RisoHash({ ppos.x + 1.0f, ppos.y }) * (1.0f / 8.0f) +
                               RisoHash({ ppos.x + 2.0f, ppos.y }) * (1.0f / 8.0f) +
                               RisoHash({ ppos.x - 1.0f, ppos.y + 1.0f }) * (1.0f / 8.0f) +
                               RisoHash({ ppos.x, ppos.y + 1.0f }) * (1.0f / 8.0f) +
                               RisoHash({ ppos.x + 1.0f, ppos.y + 1.0f }) * (1.0f / 8.0f) +
                               RisoHash({ ppos.x, ppos.y + 2.0f }) * (1.0f / 8.0f);
                    float th = h0 * 0.5f + h1 * 0.5f;
                    chDots[ch] = (edgeHalf > 0.0001f)
                        ? Smoothstep((1.0f - rawVal) - edgeHalf, (1.0f - rawVal) + edgeHalf, th)
                        : (rawVal >= (1.0f - th) ? 1.0f : 0.0f);
                } else if (p.screenType == 7) {
                    // Floyd-Steinberg clustered weighted spatial approximation
                    Vec2 ppos{ std::floor(perPlateUV.x * fW / cellPx), std::floor(perPlateUV.y * fH / cellPx) };
                    float h0 = RisoHash(ppos);
                    float h1 = RisoHash({ ppos.x + 1.0f, ppos.y }) * (7.0f / 16.0f) +
                               RisoHash({ ppos.x - 1.0f, ppos.y + 1.0f }) * (3.0f / 16.0f) +
                               RisoHash({ ppos.x, ppos.y + 1.0f }) * (5.0f / 16.0f) +
                               RisoHash({ ppos.x + 1.0f, ppos.y + 1.0f }) * (1.0f / 16.0f);
                    float th = h0 * 0.4f + h1 * 0.6f;
                    chDots[ch] = (edgeHalf > 0.0001f)
                        ? Smoothstep((1.0f - rawVal) - edgeHalf, (1.0f - rawVal) + edgeHalf, th)
                        : (rawVal >= (1.0f - th) ? 1.0f : 0.0f);
                } else {
                    // Voronoi Union Dot (Screen Type 1)
                    Vec2 gridPos = RisoComputeGridPos(uv, plateRot[ch], plateSkew[ch],
                                                      angles[ch], offX[ch], offY[ch],
                                                      freq, halftoneAspect, fW, fH);
                    float randomness = Mix(0.45f, 0.25f, Smoothstep(2.0f, 6.0f, cellPx));

                    // 2x2 supersampling
                    float pxX = 0.5f / fW;
                    float pxY = 0.5f / fH;
                    float dotAcc = 0.0f;
                    float dxs[4] = { -pxX, pxX, -pxX, pxX };
                    float dys[4] = { -pxY, -pxY, pxY, pxY };
                    for (int s = 0; s < 4; ++s) {
                        Vec2 ssUV{ uv.x + dxs[s], uv.y + dys[s] };
                        Vec2 g = RisoComputeGridPos(ssUV, plateRot[ch], plateSkew[ch],
                                                   angles[ch], offX[ch], offY[ch],
                                                   freq, halftoneAspect, fW, fH);
                        dotAcc += RisoUnionDotInk(g, p.dotShape, randomness, rawVal, 1.0f,
                                                  p.dotHardness * 0.01f, p.softness * 0.01f);
                    }
                    chDots[ch] = dotAcc * 0.25f;
                }
            }

            // ---------------------------------------------------------------
            // Pass 3: Composite, Material Simulation, Paper, Wear & Lighting
            // ---------------------------------------------------------------

            // Dot Gain & Ink Curves
            for (int ch = 0; ch < 4; ++ch) {
                if (p.dotGain > 1.001f && chDots[ch] > 0.0f) {
                    chDots[ch] = std::pow(chDots[ch], 1.0f / p.dotGain);
                }
                if (std::fabs(plateCurves[ch] - 1.0f) > 0.005f && chDots[ch] > 0.0f) {
                    chDots[ch] = std::pow(chDots[ch], 1.0f / plateCurves[ch]);
                }
            }

            // Roller Streaks
            if (p.rollerStreaks > 0.01f) {
                float rad = p.streakAngle * 0.0174532925f;
                float bandCoord = uv.y * std::cos(rad) - uv.x * std::sin(rad);
                float modulator = 1.0f + (RisoFBM({ bandCoord * p.streakFreq, 0.0f }, 3) - 0.5f) * (p.rollerStreaks * 0.01f);
                for (int ch = 0; ch < 4; ++ch) {
                    chDots[ch] = Clamp01(chDots[ch] * modulator);
                }
            }

            // Ghosting
            if (p.ghostOpacity > 0.01f && (std::fabs(p.ghostOffsetX) > 0.1f || std::fabs(p.ghostOffsetY) > 0.1f)) {
                float gFac = p.ghostOpacity * 0.01f;
                for (int ch = 0; ch < 4; ++ch) {
                    chDots[ch] = std::max(chDots[ch], chDots[ch] * (1.0f - gFac * 0.5f) + gFac * 0.15f);
                }
            }

            // TAC (Total Area Coverage) Limiting
            float totalCov = chDots[0] + chDots[1] + chDots[2] + chDots[3];
            float maxAllowed = p.totalCoverageLimit * 0.01f;
            if (totalCov > maxAllowed && totalCov > 1e-4f) {
                float scale = maxAllowed / totalCov;
                chDots[0] *= scale;
                chDots[1] *= scale;
                chDots[2] *= scale;
                chDots[3] *= scale;
            }

            // Paper Texture & Color
            ColorRGB paper = p.paperColor;
            float paperGrainVal = 0.5f;
            if (p.paperGrain > 0.01f) {
                float gScale = std::max(1.0f, p.grainScale);
                paperGrainVal = RisoFBM({ uv.x * gScale, uv.y * gScale }, 3);

                // Paper Type Specialization
                if (p.paperType == 2) { // Newsprint
                    paperGrainVal *= 1.25f;
                } else if (p.paperType == 4) { // Kraft (Horizontal Banding)
                    paperGrainVal += 0.35f * std::sin(uv.y * 350.0f);
                } else if (p.paperType == 5) { // Papyrus
                    paperGrainVal += 0.25f * RisoNoise2D({ uv.x * 20.0f, uv.y * 200.0f });
                }

                float structFac = p.paperGrain * 0.01f * (paperGrainVal - 0.5f) * 0.3f;
                paper.r = Clamp01(paper.r + structFac);
                paper.g = Clamp01(paper.g + structFac);
                paper.b = Clamp01(paper.b + structFac);
            }

            // Aging Color Mix
            if (p.agingAmount > 0.01f) {
                float edgeDist = std::min(std::min(uv.x, 1.0f - uv.x), std::min(uv.y, 1.0f - uv.y));
                float edgeVignette = Smoothstep(0.25f, 0.0f, edgeDist);
                float ageFac = p.agingAmount * 0.01f * (0.4f + 0.6f * edgeVignette);
                paper.r = Mix(paper.r, p.agingColor.r, ageFac);
                paper.g = Mix(paper.g, p.agingColor.g, ageFac);
                paper.b = Mix(paper.b, p.agingColor.b, ageFac);
            }

            // Background Mode Handling
            ColorRGB compBg = paper;
            if (p.backgroundMode == 2) { // Transparent
                compBg = { 0.0f, 0.0f, 0.0f };
            } else if (p.backgroundMode == 3) { // Original Image
                compBg = rgb;
            }

            // Plate Inks Array
            ColorRGB activeInks[4];
            if (p.colorMode == 2) {
                activeInks[0] = spotInks[0];
                activeInks[1] = spotInks[1];
                activeInks[2] = spotInks[2];
                activeInks[3] = spotInks[3];
            } else {
                activeInks[0] = p.inkCyan;
                activeInks[1] = p.inkMagenta;
                activeInks[2] = p.inkYellow;
                activeInks[3] = p.inkBlack;
            }

            // Composite Overprint
            ColorRGB compositeColor = compBg;
            if (p.overprintMode == 2) {
                // Physical Beer-Lambert Exponential Transmittance
                // T = exp( - sum( density_i * translucency_i ) )
                float transC = p.cyanTranslucency * 0.01f;
                float transM = p.magentaTranslucency * 0.01f;
                float transY = p.yellowTranslucency * 0.01f;
                float transK = p.blackTranslucency * 0.01f;

                float optR = 0.0f, optG = 0.0f, optB = 0.0f;
                float plateTrans[4] = { transC, transM, transY, transK };

                for (int ch = 0; ch < 4; ++ch) {
                    if (!plateEn[ch] || chDots[ch] <= 1e-4f) continue;
                    float cov = chDots[ch];
                    float alpha = plateTrans[ch];
                    optR += (1.0f - activeInks[ch].r) * cov * (1.2f - 0.2f * alpha);
                    optG += (1.0f - activeInks[ch].g) * cov * (1.2f - 0.2f * alpha);
                    optB += (1.0f - activeInks[ch].b) * cov * (1.2f - 0.2f * alpha);
                }

                compositeColor.r = compBg.r * std::exp(-optR);
                compositeColor.g = compBg.g * std::exp(-optG);
                compositeColor.b = compBg.b * std::exp(-optB);
            } else {
                // Multiply Overprint (Physical subtractive ink laydown)
                for (int ch = 0; ch < 4; ++ch) {
                    if (!plateEn[ch] || chDots[ch] <= 1e-4f) continue;
                    float cov = chDots[ch];
                    ColorRGB ink = activeInks[ch];
                    compositeColor.r = Mix(compositeColor.r, compositeColor.r * ink.r, cov);
                    compositeColor.g = Mix(compositeColor.g, compositeColor.g * ink.g, cov);
                    compositeColor.b = Mix(compositeColor.b, compositeColor.b * ink.b, cov);
                }
            }

            // Lighting (Artificial Lamp & Ambient/3-Point)
            if (p.lightIntensity > 0.01f) {
                float lAngleRad = p.lightAngle * 0.0174532925f;
                float lElevRad = p.lightElevation * 0.0174532925f;
                Vec3 lDir{
                    std::cos(lAngleRad) * std::cos(lElevRad),
                    std::sin(lAngleRad) * std::cos(lElevRad),
                    std::sin(lElevRad)
                };
                // Paper normal derived from grain
                Vec3 normal{ (paperGrainVal - 0.5f) * 0.3f, 0.0f, 1.0f };
                float nLen = std::sqrt(Dot3(normal, normal));
                normal.x /= nLen; normal.y /= nLen; normal.z /= nLen;

                float diff = std::max(0.0f, Dot3(normal, lDir));
                float lFactor = (p.ambientLight * 0.01f) + diff * (p.lightIntensity * 0.01f);
                compositeColor.r = Clamp01(compositeColor.r * lFactor);
                compositeColor.g = Clamp01(compositeColor.g * lFactor);
                compositeColor.b = Clamp01(compositeColor.b * lFactor);
            }

            // Reverse Print (white ink on dark)
            if (p.reversePrint) {
                compositeColor.r = 1.0f - compositeColor.r;
                compositeColor.g = 1.0f - compositeColor.g;
                compositeColor.b = 1.0f - compositeColor.b;
            }

            // Negative Output
            if (p.negativeOutput) {
                compositeColor.r = 1.0f - compositeColor.r;
                compositeColor.g = 1.0f - compositeColor.g;
                compositeColor.b = 1.0f - compositeColor.b;
            }

            // Output Gamma
            if (std::fabs(p.outputGamma - 1.0f) > 0.01f) {
                float invOutGamma = 1.0f / std::max(0.01f, p.outputGamma);
                compositeColor.r = std::pow(Clamp01(compositeColor.r), invOutGamma);
                compositeColor.g = std::pow(Clamp01(compositeColor.g), invOutGamma);
                compositeColor.b = std::pow(Clamp01(compositeColor.b), invOutGamma);
            }

            // Working Space Linear -> sRGB if needed
            if (p.workingSpace == 2) {
                compositeColor = LinearToSrgb3(compositeColor);
            }

            // ---------------------------------------------------------------
            // Proof Modes Evaluation
            // ---------------------------------------------------------------
            ColorRGB finalRgb = compositeColor;

            if (p.plateProof == 2) { // Dot Mask Only
                float m = std::max(std::max(chDots[0], chDots[1]), std::max(chDots[2], chDots[3]));
                finalRgb = { 1.0f - m, 1.0f - m, 1.0f - m };
            } else if (p.plateProof == 3) { // Cyan Separation
                finalRgb = Mix3({1, 1, 1}, activeInks[0], chDots[0]);
            } else if (p.plateProof == 4) { // Magenta Separation
                finalRgb = Mix3({1, 1, 1}, activeInks[1], chDots[1]);
            } else if (p.plateProof == 5) { // Yellow Separation
                finalRgb = Mix3({1, 1, 1}, activeInks[2], chDots[2]);
            } else if (p.plateProof == 6) { // Black Separation
                finalRgb = Mix3({1, 1, 1}, activeInks[3], chDots[3]);
            } else if (p.plateProof == 7) { // 4-Up Proof Grid
                int qx = (uv.x < 0.5f) ? 0 : 1;
                int qy = (uv.y < 0.5f) ? 0 : 1;
                // Guide crosshair lines
                float guidePx = 1.0f / std::max(fW, fH);
                if (std::fabs(uv.x - 0.5f) < guidePx || std::fabs(uv.y - 0.5f) < guidePx) {
                    finalRgb = { 0.2f, 0.2f, 0.2f };
                } else {
                    int pIdx = (qy == 0) ? (qx == 0 ? 0 : 1) : (qx == 0 ? 2 : 3);
                    finalRgb = Mix3({1, 1, 1}, activeInks[pIdx], chDots[pIdx]);
                }
            }

            // Blend With Original (Safe bypass when 0%)
            if (p.blendOriginal > 0.01f) {
                float keep = p.blendOriginal * 0.01f;
                finalRgb.r = Mix(finalRgb.r, srcSample.r, keep);
                finalRgb.g = Mix(finalRgb.g, srcSample.g, keep);
                finalRgb.b = Mix(finalRgb.b, srcSample.b, keep);
            }

            // Output write
            float* outPx = dst.at(x, y);
            outPx[0] = Clamp01(finalRgb.r);
            outPx[1] = Clamp01(finalRgb.g);
            outPx[2] = Clamp01(finalRgb.b);
            outPx[3] = (p.backgroundMode == 2) ? Clamp01(totalCov) : srcSample.a;
        }
    }
}

} // namespace resoprint
