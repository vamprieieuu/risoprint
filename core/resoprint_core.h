#ifndef RISOPRINT_CORE_H
#define RISOPRINT_CORE_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace resoprint {

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
};

struct Image {
    int w = 0;
    int h = 0;
    float* px = nullptr;
    inline float* at(int x, int y) const {
        x = std::max(0, std::min(w - 1, x));
        y = std::max(0, std::min(h - 1, y));
        return px + ((size_t)y * w + x) * 4;
    }
    inline const float* at_const(int x, int y) const {
        x = std::max(0, std::min(w - 1, x));
        y = std::max(0, std::min(h - 1, y));
        return px + ((size_t)y * w + x) * 4;
    }
};

struct InkParams {
    bool enabled = true;
    Color color{1.0f, 1.0f, 1.0f};
    double separation = 0.5; // 0..1
    double shiftX = 0.0;     // normalized image width [-0.05 .. +0.05]
    double shiftY = 0.0;     // normalized image height [-0.05 .. +0.05]
};

struct Params {
    double intensity = 1.0;  // 0..1 normalized (from 0..100% UI slider)
    int blend = 1;           // 1=Normal, 2=Darken, 3=Multiply, 4=Color Burn, 5=Lighten, 6=Screen, 7=Plus Lighter, 8=Color Dodge, 9=Overlay, 10=Soft Light, 11=Hard Light
    bool showSource = false;
    Color paper{1.0f, 1.0f, 1.0f};
    double inkContrast = -0.33;
    double scale = 0.21;
    double opacity = 0.65;
    double softness = 0.50;
    double contrast = 1.58;
    double blendOriginal = 0.0; // 0..1 (default 0% so no ghosting occurs)
    InkParams ink[3];

    Params() {
        // Default plate 1: Cyan / Aqua (#50D9F1)
        ink[0].enabled = true;
        ink[0].color = {80.0f / 255.0f, 217.0f / 255.0f, 241.0f / 255.0f};
        ink[0].separation = 0.38;
        ink[0].shiftX = 0.003;
        ink[0].shiftY = 0.002;

        // Default plate 2: Ochre / Orange (#BF7700)
        ink[1].enabled = true;
        ink[1].color = {191.0f / 255.0f, 119.0f / 255.0f, 0.0f / 255.0f};
        ink[1].separation = 0.64;
        ink[1].shiftX = 0.000;
        ink[1].shiftY = -0.001;

        // Default plate 3: Blue / Violet (#2900FF)
        ink[2].enabled = true;
        ink[2].color = {41.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f};
        ink[2].separation = 0.39;
        ink[2].shiftX = 0.000;
        ink[2].shiftY = -0.003;
    }
};

struct FrameCtx {
    double timeSec = 0.0;
};

inline float clampf(float v, float a = 0.0f, float b = 1.0f) {
    return std::max(a, std::min(b, v));
}

inline float smoothstep(float a, float b, float x) {
    if (std::abs(b - a) < 1e-6f) return x >= a ? 1.0f : 0.0f;
    float t = clampf((x - a) / (b - a));
    return t * t * (3.0f - 2.0f * t);
}

inline float luma(float r, float g, float b) {
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

inline uint32_t hash_u32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

inline uint32_t hash2(uint32_t a, uint32_t b) {
    return hash_u32(a ^ hash_u32(b + 0x9e3779b9u));
}

inline float noise01(int x, int y, int seed = 0) {
    uint32_t n = (uint32_t)x * 374761393u ^ (uint32_t)y * 668265263u ^ (uint32_t)seed * 2246822519u;
    n = hash_u32(n);
    return (n & 0x00ffffffu) / 16777216.0f;
}

void render(const Image& src, const Image& dst, const Params& p, const FrameCtx& ctx);

} // namespace resoprint

#endif // RISOPRINT_CORE_H
