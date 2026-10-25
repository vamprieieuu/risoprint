#ifndef RISOPRINT_CORE_H
#define RISOPRINT_CORE_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace resoprint {

// ---------------------------------------------------------------------------
// Basic Color Structures & Helpers
// ---------------------------------------------------------------------------

struct ColorRGB {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
};

struct ColorRGBA {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Vec4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;
};

struct ImageBuffer {
    int w = 0;
    int h = 0;
    float* data = nullptr; // interleaved RGBA float [0..1]
    int rowBytes = 0;

    inline float* at(int x, int y) {
        x = std::max(0, std::min(w - 1, x));
        y = std::max(0, std::min(h - 1, y));
        return data + ((size_t)y * w + x) * 4;
    }

    inline const float* at_const(int x, int y) const {
        x = std::max(0, std::min(w - 1, x));
        y = std::max(0, std::min(h - 1, y));
        return data + ((size_t)y * w + x) * 4;
    }
};

// ---------------------------------------------------------------------------
// Spot Inks & Presets Definitions
// ---------------------------------------------------------------------------

struct SpotInkDef {
    const char* name;
    ColorRGB rgb;
};

extern const SpotInkDef SPOT_INKS_CATALOG[];
extern const int SPOT_INKS_COUNT;

ColorRGB GetSpotInkRgbByIndex(int index);

// ---------------------------------------------------------------------------
// Plugin Parameter Block (Complete Reference Inventory)
// ---------------------------------------------------------------------------

struct PluginParams {
    // Top-Level
    int shuffleSeed = 0;
    int pressPreset = 1; // 1 = Custom, 2..29 = Presets

    // Group: Inspect
    int plateProof = 1;         // 1=Off, 2=Dot Mask, 3=Cyan, 4=Magenta, 5=Yellow, 6=Black, 7=4-Up Proof Grid
    float magnifyZoom = 1.0f;   // 1.0 .. 10.0
    float magnifyCenterX = 50.0f; // 0 .. 100%
    float magnifyCenterY = 50.0f; // 0 .. 100%
    bool cyanPlateEn = true;
    bool magentaPlateEn = true;
    bool yellowPlateEn = true;
    bool blackPlateEn = true;
    bool haloHalfRes = false;

    // Group: Machine & Paper
    int machineProfile = 1;     // 1=Custom, 2..12 = Machine models
    int printResolution = 1;    // 1=Custom (use Scale), 2=150 DPI, 3=300 DPI, 4=600 DPI, 5=1200 DPI
    int paperType = 1;          // 1=Custom, 2=Newsprint, 3=Cotton Rag, 4=Kraft, 5=Papyrus, 6=Smooth Coated
    int backgroundMode = 1;     // 1=Paper, 2=Transparent, 3=Original Image
    int spotInkKit = 1;         // 1=Custom, 2..9 = Kits
    int workingSpace = 1;       // 1=Passthrough, 2=Linearize sRGB

    // Group: Color Correction
    float brightness = 0.0f;    // -100 .. +100
    float contrast = 0.0f;      // -100 .. +100
    float hue = 0.0f;           // -180 .. +180 deg
    float saturation = 0.0f;    // -100 .. +100
    float hsvValue = 0.0f;      // -100 .. +100
    float gamma = 1.00f;        // 0.1 .. 3.0

    // Group: Ink Palette
    int colorMode = 1;          // 1=CMYK, 2=Spot Riso
    int spotSeparation = 1;     // 1=Greedy (Legacy), 2=Balanced Mix
    int lineArtPlate = 1;       // 1=Off, 2=Black/K, 3=Plate 1, 4=Plate 2, 5=Plate 3, 6=Plate 4
    float lineArtThreshold = 55.0f; // 0 .. 100
    float lineArtStrength = 100.0f; // 0 .. 100
    int activePlates = 4;       // 2 .. 4
    float coverage = 100.0f;    // 0 .. 200%

    // CMYK Inks
    ColorRGB inkCyan{0.0f, 0.627f, 0.914f};     // #00A0E9
    ColorRGB inkMagenta{0.894f, 0.0f, 0.498f};  // #E4007F
    ColorRGB inkYellow{1.0f, 0.945f, 0.0f};     // #FFF100
    ColorRGB inkBlack{0.137f, 0.094f, 0.082f};  // #231815

    // Spot Inks (Selected by index or overridden)
    int spotInk1 = 1;
    int spotInk2 = 1;
    int spotInk3 = 1;
    int spotInk4 = 1;
    ColorRGB customSpotInks[4] = {
        {0.314f, 0.851f, 0.945f}, // Plate 1 #50D9F1
        {0.749f, 0.467f, 0.000f}, // Plate 2 #BF7700
        {0.161f, 0.000f, 1.000f}, // Plate 3 #2900FF
        {0.100f, 0.100f, 0.100f}  // Plate 4 Black
    };

    float blackBoost = 100.0f;   // 0 .. 200%
    float risoTint = 0.0f;       // 0 .. 100%
    int overprintMode = 1;       // 1=Multiply, 2=Beer-Lambert
    float cyanTranslucency = 85.0f;
    float magentaTranslucency = 85.0f;
    float yellowTranslucency = 85.0f;
    float blackTranslucency = 85.0f;
    float totalCoverageLimit = 300.0f; // 100 .. 400%
    float inkPooling = 0.0f;     // 0 .. 100%
    float haloRadius = 0.0f;     // 0 .. 50 px
    float haloStrength = 0.0f;   // 0 .. 100%
    float cyanCurve = 1.00f;
    float magentaCurve = 1.00f;
    float yellowCurve = 1.00f;
    float blackCurve = 1.00f;

    // Group: Halftone
    int screenType = 1;         // 1=Voronoi, 2=Bayer, 3=Blue-noise, 4=Line, 5=Stippled, 6=Atkinson, 7=Floyd
    float screenDensity = 200.0f; // 10 .. 600
    int dotShape = 1;           // 1=Circle, 2=Ellipse, 3=Diamond, 4=Square
    float dotHardness = 110.0f; // 0 .. 200%
    float softness = 23.0f;     // 0 .. 100%
    float distortion = 0.0f;    // 0 .. 100%
    float dotStretch = 0.0f;    // -100 .. +100%
    float edgeSoftness = 30.0f; // 0 .. 100%
    float dotGain = 1.00f;      // 0.5 .. 3.0
    float angleCyan = 15.0f;
    float angleMagenta = 75.0f;
    float angleYellow = 0.0f;
    float angleBlack = 45.0f;
    float weightCyan = 128.0f;
    float weightMagenta = 109.0f;
    float weightYellow = 100.0f;
    float weightBlack = 112.0f;
    float drumAnimSpeed = 0.0f;
    float richBlacks = 0.0f;

    // Group: Plate Alignment
    float allOffsetX = 0.0f;    // -100 .. +100 px
    float allOffsetY = 0.0f;    // -100 .. +100 px
    float allRotation = 0.0f;   // -180 .. +180 deg
    float allSkew = 0.0f;       // -1.0 .. +1.0
    float cyanOffsetX = 0.0f;
    float cyanOffsetY = 0.0f;
    float magentaOffsetX = 0.0f;
    float magentaOffsetY = 0.0f;
    float yellowOffsetX = 0.0f;
    float yellowOffsetY = 0.0f;
    float blackOffsetX = 0.0f;
    float blackOffsetY = 0.0f;
    float cyanRotation = 0.0f;
    float magentaRotation = 0.0f;
    float yellowRotation = 0.0f;
    float blackRotation = 0.0f;
    float cyanSkew = 0.0f;
    float magentaSkew = 0.0f;
    float yellowSkew = 0.0f;
    float blackSkew = 0.0f;

    // Group: Paper & Grain
    ColorRGB paperColor{0.980f, 0.969f, 0.941f}; // #FAF7F0
    float paperGrain = 20.0f;   // 0 .. 100%
    float grainScale = 50.0f;   // 1 .. 200%
    float sheetWarp = 15.0f;    // 0 .. 100%
    float agingAmount = 15.0f;  // 0 .. 100%
    ColorRGB agingColor{0.541f, 0.353f, 0.169f}; // #8A5A2B

    // Group: Print Wear
    float sunBleach = 0.0f;     // 0 .. 100%
    float bleachDirection = 0.0f; // 0 .. 360 deg
    float inkVariance = 15.0f;  // 0 .. 100%
    float inkFlecks = 10.0f;    // 0 .. 100%
    float fleckScale = 50.0f;   // 1 .. 200%
    float inkBlots = 5.0f;      // 0 .. 100%
    float rollerStreaks = 0.0f; // 0 .. 100%
    float streakFreq = 12.0f;   // 1 .. 50
    float streakAngle = 90.0f;  // 0 .. 360 deg
    float ghostOffsetX = 0.0f;  // -50 .. +50 px
    float ghostOffsetY = 0.0f;  // -50 .. +50 px
    float ghostOpacity = 0.0f;  // 0 .. 100%
    float inkReticulation = 0.0f; // 0 .. 100%
    float reticScale = 60.0f;   // 10 .. 200 cells
    int impressions = 1;        // 1 .. 10
    float impressionJitter = 0.0f; // 0 .. 100%
    float inkDrift = 0.0f;      // 0 .. 100%
    bool reversePrint = false;  // white on dark

    // Group: Lighting
    float lightIntensity = 0.0f; // 0 .. 100%
    float lightAngle = 45.0f;   // 0 .. 360 deg
    float lightElevation = 45.0f; // 0 .. 90 deg
    float ambientLight = 50.0f; // 0 .. 100%
    float threePointLight = 30.0f; // 0 .. 100%

    // Group: Output
    float outputGamma = 1.30f;  // 0.5 .. 2.5
    int outputQuality = 3;      // 1=Draft, 2=Standard, 3=High, 4=Ultra
    float pressFidelity = 10.0f; // 0 .. 100%
    int softProof = 1;          // 1=Off, 2=News, 3=Uncoated, 4=Heavy Cardstock
    bool negativeOutput = false;

    // Legacy parameter for backward safety & zero ghosting
    float blendOriginal = 0.0f; // 0 .. 100% (default 0%)
};

// ---------------------------------------------------------------------------
// Math & Core Kernel Functions
// ---------------------------------------------------------------------------

inline float Clamp01(float v) {
    return std::max(0.0f, std::min(1.0f, v));
}

inline float Smoothstep(float e0, float e1, float x) {
    float t = Clamp01((x - e0) / std::max(e1 - e0, 1e-9f));
    return t * t * (3.0f - 2.0f * t);
}

inline float Mix(float a, float b, float t) {
    return a + (b - a) * t;
}

inline Vec2 Mix2(Vec2 a, Vec2 b, float t) {
    return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
}

inline Vec3 Mix3(Vec3 a, Vec3 b, float t) {
    return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t };
}

inline ColorRGB MixRGB(ColorRGB a, ColorRGB b, float t) {
    return { a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t };
}

inline ColorRGB Mix(ColorRGB a, ColorRGB b, float t) {
    return { a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t };
}

inline float Luma3(ColorRGB c) {
    return c.r * 0.2126f + c.g * 0.7152f + c.b * 0.0722f;
}

// Linear <-> sRGB
inline float SrgbToLinearF(float c) {
    c = Clamp01(c);
    return (c < 0.04045f) ? (c / 12.92f) : std::pow((c + 0.055f) / 1.055f, 2.4f);
}

inline float LinearToSrgbF(float c) {
    c = Clamp01(c);
    return (c < 0.0031308f) ? (c * 12.92f) : (1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f);
}

inline ColorRGB SrgbToLinear3(ColorRGB c) {
    return { SrgbToLinearF(c.r), SrgbToLinearF(c.g), SrgbToLinearF(c.b) };
}

inline ColorRGB LinearToSrgb3(ColorRGB c) {
    return { LinearToSrgbF(c.r), LinearToSrgbF(c.g), LinearToSrgbF(c.b) };
}

// Color Space Transformations (RGB <-> HSV)
Vec3 RGBToHSV(ColorRGB rgb);
ColorRGB HSVToRGB(Vec3 hsv);

// Procedural Hash & Noise
float RisoHash(Vec2 p);
Vec2  RisoHash2(Vec2 p);
float RisoHash3(Vec3 p);
float RisoNoise2D(Vec2 p);
float RisoNoise3D(Vec3 p);
float RisoFBM(Vec2 p, int octaves);
float RisoFBM3D(Vec3 p, int octaves);

// Halftone Geometry & Distance Functions
float RisoCellDist(Vec2 d, int shape);
void  RisoVoronoiCell(Vec2 p, int shape, float randomness, float* outDist, Vec2* outCenter);
float RisoApplyThreshold(float dist, float channelVal, float factor, float intensity, float smoothness);
float RisoUnionDotInk(Vec2 gridPos, int shape, float randomness, float channelVal, float factor, float intensity, float smoothness);

// Screen Type Threshold Evaluators
float RisoBayer8Value(int ix, int iy);
float RisoPseudoBlueNoise(Vec2 p);

// Transforms
Vec2 RisoApplyPlateXform(Vec2 uv, float plateRotRad, float plateSkew, float resW, float resH);
Vec2 RisoApplyPlateXformInverse(Vec2 uv, float plateRotRad, float plateSkew, float resW, float resH);
Vec2 RisoComputeGridPos(Vec2 uv, float plateRotRad, float plateSkew, float angleRad, float offX, float offY, float freq, float aspect, float resW, float resH);
Vec2 RisoGridToUV(Vec2 gridPos, float plateRotRad, float plateSkew, float angleRad, float offX, float offY, float freq, float aspect, float resW, float resH);

// Line Art Extraction
float RisoLineArtMask(float centerLuma, float edgeContrast, float threshold01, float strength01);

// Spot Decomposition
Vec4 RisoSpotGreedyDecompose(Vec3 target, Vec3 a0, Vec3 a1, Vec3 a2, Vec3 a3, int spotPlateCount);
Vec4 RisoSpotBalancedDecompose(Vec3 target, Vec3 a0, Vec3 a1, Vec3 a2, Vec3 a3, int spotPlateCount);

// Presets Applicators
void ApplyPressPreset(int presetIdx, PluginParams& p);
void ApplyMachineProfile(int profileIdx, PluginParams& p);
void ApplySpotKit(int kitIdx, PluginParams& p);

// ---------------------------------------------------------------------------
// Main Rendering Engine (Full 3-Pass CPU Pipeline)
// ---------------------------------------------------------------------------

void RenderRisoprint(
    const ImageBuffer& src,
    ImageBuffer& dst,
    const PluginParams& params,
    double frameTimeSec = 0.0
);

} // namespace resoprint

#endif // RISOPRINT_CORE_H
