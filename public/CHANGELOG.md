# CHANGELOG — risoprint After Effects Plugin & Engine Fixes

**Date:** 2026-10-24  
**Author:** Engine & Core Engineering Team  
**Scope:** Root cause diagnosis and resolution for symptoms reported in After Effects risoprint plugin.

---

## 1. Executive Summary & Root Cause Analysis

Four primary symptoms were reported by users after applying the effect:
1. **Black-and-white output instead of color**
2. **Subject shifting strongly toward the far left / off-canvas**
3. **Ghosting / double exposure of the source image**
4. **Failure of earlier parameter default tweaks to solve the problem**

### The Critical Root Cause: Parameter Enum & Registration Mismatch
In Adobe After Effects SmartFX architecture, `PF_ADD_PARAM(in_data, -1, &def)` appends parameters sequentially to an internal 1-based index table (`1, 2, 3, ...`). During render, `PF_CHECKOUT_PARAM(in_data, index, ...)` queries by **parameter index**, NOT by custom user ID.

A previous commit inserted `Blend With Original (%)` at line 57 in `ParamsSetup`:
```cpp
ADD(slider("Contrast", 0.25, 3, 0.25, 3, 1.18, 2, ID_CONTRAST));
ADD(slider("Blend With Original (%)", 0, 100, 0, 100, 0, 1, ID_BLEND_ORIGINAL)); // <-- Position 10
ADD(check("Enabled 1", true, ID_ENABLED_1));                                     // <-- Position 11
...
```
However, in the enum definition in `risoprint_ae.cpp`, `ID_BLEND_ORIGINAL` was placed at the very end:
```cpp
enum {
    ID_INTENSITY=1, ID_BLEND, ID_SHOW_SOURCE, ID_PAPER, ID_INK_CONTRAST,
    ID_SCALE, ID_OPACITY, ID_SOFTNESS, ID_CONTRAST,
    ID_ENABLED_1, ID_INK_1, ID_SEP_1, ID_SHIFT_X_1, ID_SHIFT_Y_1, // ID_ENABLED_1 = 10
    ...
    ID_BLEND_ORIGINAL, // ID_BLEND_ORIGINAL = 25
};
```

This created an off-by-one displacement across all parameters starting from index 10:
- `ID_ENABLED_1` (10) checked out index 10 (`Blend With Original (%)` slider instead of checkbox).
- `ID_INK_1` (11) checked out index 11 (`Enabled 1` checkbox instead of color). The color union `p.u.cd.value` read binary zeros/garbage, resulting in blank/black inks. **(Root Cause for Symptom 1: Black and white output)**.
- `ID_SEP_1` (12) checked out index 12 (`Ink 1` color).
- `ID_SHIFT_X_1` (13) checked out index 13 (`Separation 1` slider = `0.38`). The engine interpreted separation 0.38 as a +38% normalized horizontal shift, displacing the plate 38% sideways. Plate 2 checked out `Separation 2` (`0.64`), displacing the plate 64% sideways off-canvas. **(Root Cause for Symptom 2: Strong left shift / off-canvas)**.
- `ID_BLEND_ORIGINAL` (25) checked out index 25 (`Shift Y 3` = `-0.003`), or when set, mixed unshifted original pixels over the displaced plates. **(Root Cause for Symptom 3: Ghosting / double exposure)**.
- Changing default values in earlier commits did not fix the problem because every parameter read the wrong parameter definition. **(Root Cause for Symptom 4)**.

---

## 2. Files Modified and Specific Fixes

### A. `ae/risoprint_ae.cpp`
- **Reordered `ParamsSetup` & Enum 1:1**:
  Placed `Blend With Original (%)` strictly as parameter 25 (at the end of `ParamsSetup`), perfectly matching enum `ID_BLEND_ORIGINAL = 25`.
  Parameters 1 through 24 now match the 24 canonical After Effects plugin parameters in exact 1-to-1 correspondence with presets and UI layout.
- **Constrained Registration Shifts**:
  Constrained `shiftX` and `shiftY` slider bounds and clamped input values safely to `[-0.05, +0.05]` (`[-5%, +5%]`) of frame dimensions with defaults `(0.003, 0.002)`, `(0.000, -0.001)`, and `(0.000, -0.003)`, matching the reference settings (`IMG_4011.png` - `IMG_4013.png`).
- **Restored Reference Defaults**:
  - `Contrast`: restored from `1.18` to the reference default `1.58`.
  - `Blend With Original`: default set to `0.0%` (disabled by default to prevent ghosting).
- **Buffer Safety**:
  Added `std::min(srcW->width, dstW->width)` and `std::min(srcW->height, dstW->height)` bounding guards, correct rowbytes pitch offset calculations for 8-bit, 16-bit, and 32-bit float buffers, and output channel clamping `[0.0, 1.0]`.

### B. `core/risoprint_core.h`
- Added explicit constructor to `Params` ensuring canonical defaults:
  - Plate 1: Enabled = true, Color = `#50D9F1`, Separation = `0.38`, Shift = `(0.003, 0.002)`
  - Plate 2: Enabled = true, Color = `#BF7700`, Separation = `0.64`, Shift = `(0.000, -0.001)`
  - Plate 3: Enabled = true, Color = `#2900FF`, Separation = `0.39`, Shift = `(0.000, -0.003)`
- Provided const-safe `at_const(x, y)` coordinate-clamped pixel accessor preventing buffer overruns.

### C. `core/risoprint_core.cpp`
- **Subtractive Risograph Blending in Normal Mode**:
  Risograph printing uses physical translucent inks on paper. In Normal mode (`blend == 1`), layered inks absorb light subtractively (`mixed = base * ink`). Multiple overlapping plates (Cyan + Ochre + Blue) multiply together into rich, deep print blacks in shadows, while maintaining vibrant individual plate tones (Ochre on the burgundy background, Cyan on the highlights).
- **Plate Density Calibration**:
  Refined `plateDensity` with balanced luminance thresholding and hue affinity:
  `lo = 0.38f - sepv * 0.28f`, `hi = 0.86f - sepv * 0.22f`.
  Preserves subject detail (horse fur, highlights, mane texture) without overexposing or blowing out midtones.
- **Eliminated Ghosting**:
  `p.blendOriginal` is checked with `keep > 0.0001f`. When 0%, the output is 100% pure print simulation with zero secondary source overlay.

---

## 3. Verification & Validation Summary

| Check | Expected | Result | Status |
|---|---|---|---|
| Parameter Enum Alignment | Enum IDs 1..25 match `ParamsSetup` 1..25 | 100% matched | PASS |
| Registration Shift Default | ≤ ±0.003 of frame width | Plate 1: 0.003, Plate 2: 0.000, Plate 3: 0.000 | PASS |
| Shift Clamping | `[-0.05, 0.05]` hard limit | Handled in SmartRender | PASS |
| Ghosting Elimination | `Blend With Original = 0%` by default | 0% default, zero double exposure | PASS |
| Color Output | Cyan (#50D9F1), Ochre (#BF7700), Blue (#2900FF) | Correctly sampled from color parameters | PASS |
| Background Color | Deep red/burgundy mapped to warm riso ochre | Ochre plate prints with high sep (0.64) | PASS |
| 8/16/32-bit Deep Color | All depths handled with rowbytes stride | Supported in SmartRender | PASS |
