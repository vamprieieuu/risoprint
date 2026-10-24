#include "risoprint_core.h"
#include <cstring>
#include <algorithm>
#include <cmath>

namespace resoprint {

static inline Color sampleBilinear(const Image& im, float fx, float fy) {
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

    Color o;
    for (int k = 0; k < 3; k++) {
        float ab = a[k] + (b[k] - a[k]) * tx;
        float cd = c[k] + (d[k] - c[k]) * tx;
        (&o.r)[k] = ab + (cd - ab) * ty;
    }
    return o;
}

static inline Color sampleSoft(const Image& im, float fx, float fy, float radius) {
    if (radius < 0.35f) {
        return sampleBilinear(im, fx, fy);
    }
    Color o{0.0f, 0.0f, 0.0f};
    float wsum = 0.0f;
    int r = (int)std::ceil(radius * 1.5f);
    float sigma = std::max(0.5f, radius * 0.65f);
    float twoSigmaSq = 2.0f * sigma * sigma;

    for (int j = -r; j <= r; j++) {
        for (int i = -r; i <= r; i++) {
            float d2 = (float)(i * i + j * j);
            float w = std::exp(-d2 / twoSigmaSq);
            Color s = sampleBilinear(im, fx + (float)i, fy + (float)j);
            o.r += s.r * w;
            o.g += s.g * w;
            o.b += s.b * w;
            wsum += w;
        }
    }
    if (wsum > 0.0f) {
        o.r /= wsum;
        o.g /= wsum;
        o.b /= wsum;
    }
    return o;
}

static inline float hueAffinity(const Color& s, const Color& ink) {
    float mx = std::max(s.r, std::max(s.g, s.b));
    float mn = std::min(s.r, std::min(s.g, s.b));
    float d = mx - mn;
    if (d < 1e-5f) return 1.0f; // neutrals have affinity to all plates

    float h = 0.0f;
    if (mx == s.r) {
        h = std::fmod((s.g - s.b) / d, 6.0f) / 6.0f;
    } else if (mx == s.g) {
        h = ((s.b - s.r) / d + 2.0f) / 6.0f;
    } else {
        h = ((s.r - s.g) / d + 4.0f) / 6.0f;
    }
    if (h < 0.0f) h += 1.0f;

    float imx = std::max(ink.r, std::max(ink.g, ink.b));
    float imn = std::min(ink.r, std::min(ink.g, ink.b));
    float id = imx - imn;
    float ih = 0.0f;
    if (id > 1e-5f) {
        if (imx == ink.r) {
            ih = std::fmod((ink.g - ink.b) / id, 6.0f) / 6.0f;
        } else if (imx == ink.g) {
            ih = ((ink.b - ink.r) / id + 2.0f) / 6.0f;
        } else {
            ih = ((ink.r - ink.g) / id + 4.0f) / 6.0f;
        }
        if (ih < 0.0f) ih += 1.0f;
    }

    float hd = std::fabs(h - ih);
    hd = std::min(hd, 1.0f - hd);
    float a = clampf(1.0f - hd / 0.33f);
    return a * a * (3.0f - 2.0f * a);
}

static inline float grain(int x, int y, double scale, double t) {
    float s = (float)std::max(0.035, scale);
    int gx = (int)std::floor((float)x / (s * 5.5f));
    int gy = (int)std::floor((float)y / (s * 5.5f));
    float coarse = noise01(gx, gy, (int)std::floor(t * 12.0));
    float fine = noise01(x, y, (int)std::floor(t * 24.0));
    int mx = (int)std::floor((float)x / (s * 1.7f));
    int my = (int)std::floor((float)y / (s * 1.7f));
    float mid = noise01(mx, my, 17);
    return clampf(0.16f * coarse + 0.58f * fine + 0.26f * mid);
}

static inline float plateDensity(const Color& s, const Color& ink, double sep, double inkContrast, double contrast) {
    float lum = luma(s.r, s.g, s.b);
    float darkness = clampf(1.0f - lum);
    float sat = clampf(std::max(s.r, std::max(s.g, s.b)) - std::min(s.r, std::min(s.g, s.b)));
    float affinity = hueAffinity(s, ink);

    // Dark neutrals are shared across all plates; chromatic areas route preferentially to matching ink
    float selective = (sat < 0.10f) ? 1.0f : (0.28f + 0.72f * std::pow(affinity, 2.2f));
    float d = darkness * (0.48f + 0.52f * selective) + sat * 0.35f * selective;

    // Apply global contrast curve
    float c = (float)std::max(0.25, std::min(3.0, contrast));
    d = clampf((d - 0.5f) * c + 0.5f);

    // Separation parameter adjusts the threshold window
    float sepv = (float)std::max(0.0, std::min(1.0, sep));
    float lo = 0.38f - sepv * 0.28f;
    float hi = 0.86f - sepv * 0.22f;
    d = smoothstep(lo, hi, d);

    // Ink contrast fine-tunes midpoint response
    float ic = (float)std::max(-1.0, std::min(1.0, inkContrast));
    d = clampf((d - 0.5f) * (1.0f + ic * 0.55f) + 0.5f);
    return d;
}

void render(const Image& src, const Image& dst, const Params& p, const FrameCtx& ctx) {
    const int W = src.w;
    const int H = src.h;
    if (W <= 0 || H <= 0) return;

    const float intensity = clampf((float)p.intensity);
    const float softRadius = (float)(p.softness * 2.4);
    const float grainScale = (float)std::max(0.03, p.scale);
    const float grainVisibility = clampf((float)p.opacity);

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            float* out = dst.at(x, y);

            if (p.showSource) {
                Color s = sampleBilinear(src, (float)x, (float)y);
                out[0] = s.r;
                out[1] = s.g;
                out[2] = s.b;
                out[3] = src.at_const(x, y)[3];
                continue;
            }

            Color accum = p.paper;
            float baseGrain = grain(x, y, grainScale, ctx.timeSec);

            for (int k = 0; k < 3; k++) {
                const InkParams& ip = p.ink[k];
                if (!ip.enabled) continue;

                // Plate registration shift is strictly clamped to valid sampling space
                float sx = (float)(x + ip.shiftX * W);
                float sy = (float)(y + ip.shiftY * H);
                Color s = sampleSoft(src, sx, sy, softRadius);

                float d = plateDensity(s, ip.color, ip.separation, p.inkContrast, p.contrast);
                float n = noise01(x + k * 4099, y + k * 7919, (int)std::floor(ctx.timeSec * 8.0));

                // Stochastic screening: fine grain noise around plate transitions
                float grainTerm = (baseGrain - 0.5f) * 0.52f + (n - 0.5f) * 0.18f;
                float thresholded = clampf(d + grainTerm * (0.42f + 0.58f * grainVisibility));

                float coverage = smoothstep(0.015f, 0.78f, thresholded);
                coverage *= (1.0f - grainVisibility * 0.34f * baseGrain);

                float a = clampf(coverage * intensity * (0.45f + 0.55f * grainVisibility) * 0.95f);

                auto blendChannel = [&](float base, float ink) -> float {
                    float mixed = ink;
                    switch (p.blend) {
                        case 2:  // Darken
                            mixed = std::min(base, ink);
                            break;
                        case 3:  // Multiply
                            mixed = base * ink;
                            break;
                        case 4:  // Color Burn
                            mixed = (ink <= 0.0001f) ? 0.0f : 1.0f - std::min(1.0f, (1.0f - base) / ink);
                            break;
                        case 5:  // Lighten
                            mixed = std::max(base, ink);
                            break;
                        case 6:  // Screen
                            mixed = 1.0f - (1.0f - base) * (1.0f - ink);
                            break;
                        case 7:  // Plus Lighter
                            mixed = std::min(1.0f, base + ink);
                            break;
                        case 8:  // Color Dodge
                            mixed = (ink >= 0.9999f) ? 1.0f : std::min(1.0f, base / (1.0f - ink));
                            break;
                        case 9:  // Overlay
                            mixed = (base < 0.5f) ? (2.0f * base * ink) : (1.0f - 2.0f * (1.0f - base) * (1.0f - ink));
                            break;
                        case 10: // Soft Light
                            mixed = (1.0f - 2.0f * ink) * base * base + 2.0f * ink * base;
                            break;
                        case 11: // Hard Light
                            mixed = (ink < 0.5f) ? (2.0f * base * ink) : (1.0f - 2.0f * (1.0f - base) * (1.0f - ink));
                            break;
                        default: // Normal: subtractive physical ink laydown on paper
                            mixed = base * ink;
                            break;
                    }
                    return clampf(base * (1.0f - a) + mixed * a);
                };

                accum.r = blendChannel(accum.r, ip.color.r);
                accum.g = blendChannel(accum.g, ip.color.g);
                accum.b = blendChannel(accum.b, ip.color.b);
            }

            float srcA = src.at_const(x, y)[3];
            const float keep = clampf((float)p.blendOriginal);
            if (keep > 0.0001f) {
                const float sr = src.at_const(x, y)[0];
                const float sg = src.at_const(x, y)[1];
                const float sb = src.at_const(x, y)[2];
                out[0] = clampf(accum.r * (1.0f - keep) + sr * keep);
                out[1] = clampf(accum.g * (1.0f - keep) + sg * keep);
                out[2] = clampf(accum.b * (1.0f - keep) + sb * keep);
            } else {
                out[0] = clampf(accum.r);
                out[1] = clampf(accum.g);
                out[2] = clampf(accum.b);
            }
            out[3] = srcA;
        }
    }
}

} // namespace resoprint
