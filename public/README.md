# risoprint — Native After Effects Risograph Effect Engine

`risoprint` is a native Windows x64 `.aex` effect for Adobe After Effects (2023 / 23.x+). It simulates the distinctive look of physical 3-drum risograph and multi-ink screenprinting with real color-aware plate separation, stochastic stipple grain, and controlled micro-registration drift.

---

## Technical Audit & Fixes (2026-10-24)

### 1. Root Cause Resolution for User-Reported Symptoms
A previous commit inserted `Blend With Original (%)` at index 10 in `ParamsSetup` while leaving `ID_BLEND_ORIGINAL` at the end of the parameter enum (`ID_BLEND_ORIGINAL = 25`). Because After Effects' `PF_CHECKOUT_PARAM` queries by sequential index, every parameter from index 10 onward was displaced by one:
- **Black-and-White Output**: `ID_INK_1` queried index 11 (`Enabled 1` checkbox). Color unions read boolean zeros/garbage, killing plate color. **(Fixed)**
- **Left Canvas Shift**: `ID_SHIFT_X_1` queried index 13 (`Separation 1` slider = 0.38), displacing the cyan plate by 38% of the frame width. Plate 2 queried `Separation 2` (0.64), pushing the subject completely off-canvas. **(Fixed)**
- **Source Ghosting**: `Blend With Original` was previously set to 12% to mask broken thresholds, superimposing an unshifted source copy over the displaced plates. **(Fixed: set to 0.0% default with conditional bypass)**
- **Stable Parameter Table**: Parameters 1 through 25 are now in strict 1:1 agreement between `enum` and `ParamsSetup`.

---

## Parameter Table

| Index | Name | Type | Range / Choices | Default | Notes |
|---|---|---|---|---|---|
| 1 | Intensity | Float Slider | 0.0 .. 100.0% | 100.0% | Master ink strength |
| 2 | Blend | Popup | 11 blend modes | Normal (1) | Physical subtractive ink laydown |
| 3 | Show source | Checkbox | true / false | false | Raw source layer passthrough |
| 4 | Paper | Color | RGB [0..255] | #FFFFFF | Paper stock color |
| 5 | Ink contrast | Float Slider | -1.00 .. 1.00 | -0.33 | Halftone midpoint curve |
| 6 | Scale | Float Slider | 0.03 .. 1.00 | 0.21 | Stochastic grain screen size |
| 7 | Opacity | Float Slider | 0.00 .. 1.00 | 0.65 | Paper break-through response |
| 8 | Softness | Float Slider | 0.00 .. 1.00 | 0.50 | Optical spread filter |
| 9 | Contrast | Float Slider | 0.25 .. 3.00 | 1.58 | Separation steepness curve |
| 10 | Enabled 1 | Checkbox | true / false | true | Cyan plate toggle |
| 11 | Ink 1 | Color | RGB [0..255] | #50D9F1 | Cyan / Aqua plate |
| 12 | Separation 1 | Float Slider | 0.00 .. 1.00 | 0.38 | Plate thresholding window |
| 13 | Shift X 1 | Float Slider | -0.05 .. 0.05 | 0.003 | Micro registration chatter X |
| 14 | Shift Y 1 | Float Slider | -0.05 .. 0.05 | 0.002 | Micro registration chatter Y |
| 15 | Enabled 2 | Checkbox | true / false | true | Ochre plate toggle |
| 16 | Ink 2 | Color | RGB [0..255] | #BF7700 | Warm Ochre / Orange plate |
| 17 | Separation 2 | Float Slider | 0.00 .. 1.00 | 0.64 | Captures dark burgundy reds |
| 18 | Shift X 2 | Float Slider | -0.05 .. 0.05 | 0.000 | Micro registration chatter X |
| 19 | Shift Y 2 | Float Slider | -0.05 .. 0.05 | -0.001 | Micro registration chatter Y |
| 20 | Enabled 3 | Checkbox | true / false | true | Blue plate toggle |
| 21 | Ink 3 | Color | RGB [0..255] | #2900FF | Blue / Violet plate |
| 22 | Separation 3 | Float Slider | 0.00 .. 1.00 | 0.39 | Plate thresholding window |
| 23 | Shift X 3 | Float Slider | -0.05 .. 0.05 | 0.000 | Micro registration chatter X |
| 24 | Shift Y 3 | Float Slider | -0.05 .. 0.05 | -0.003 | Micro registration chatter Y |
| 25 | Blend With Original | Float Slider | 0.0 .. 100.0% | 0.0% | Default 0% (zero ghosting) |

---

## Build System & Verification

### Windows x64 MSVC (GitHub Actions)
Build is handled by `.github/workflows/build.yml` running on `windows-2022`:
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target risoprint --parallel
python ae/verify_build.py dist/risoprint.aex
```

### Verification Suite
Run the automated parameter alignment and safety audit:
```bash
python3 tests/verify_plugin_code.py
```

### Interactive Web Test Harness
Run the live simulator on port 3000:
```bash
npm run dev
```
Provides split-screen wipe comparison with `IMG_4003.jpeg` and `IMG_4010.jpeg`, stencil plate isolations, bug simulation, and one-click ZIP packaging.
