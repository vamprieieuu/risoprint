# RisoPrint — Native Adobe After Effects Risograph Effect Engine

`risoprint` (with dual-target `resoprint`) is a native Windows x64 `.aex` effect plugin for Adobe After Effects (2023 / 23.2.1+). It independently reimplements the complete observable behavior, visual pipeline, and parameter hierarchy of the reference `Risograph` plugin.

---

## 1. Target Hardware & Host Environment

The plugin is engineered specifically for reliable, crash-free execution on modest workstation hardware:
- **Operating System**: Windows 11 x64 (Build 22000+)
- **Host Application**: Adobe After Effects 23.2.1 (After Effects 2023)
- **Target GPU / CPU**: Intel UHD Graphics 620 / Intel Core i5/i7 (8th Gen+), 8 GB RAM
- **Design Constraint**: **Robust CPU-compatible rendering**. The reference plugin suffered from OpenCL kernel timeouts, vendor-specific driver crashes, and out-of-memory states on integrated Intel graphics. `risoprint` executes an optimized, cache-friendly CPU SIMD/OpenMP pipeline handling full 8-bit, 16-bit, and 32-bit float deep color (`PF_PixelFormat_ARGB32FP`, `PF_Pixel16`, `PF_Pixel8`) without requiring dedicated GPU VRAM.

---

## 2. Multi-Pass Processing Architecture

Reconstructed directly from the reference database analysis (`Risograph.aex.rzdb`, `Risograph_DB_Embedded_Code_Strings.txt`, and user screenshots):

```
[Source Layer (8/16/32-bit Float)]
              │
              ▼
   ┌────────────────────────────────────────────────────────┐
   │ PASS 1: Pre-process & Palette Decomposition           │
   │ - Color space conversion (sRGB / Linear Rec.709)       │
   │ - Exposure / Brightness / Contrast / Saturation / HSV  │
   │ - Sobel edge filter & Line Art key plate extraction    │
   │ - Palette Separation: CMYK or Spot Riso                │
   │   (Greedy 3-Ink or Balanced Mix 4-Sweep Decomposition) │
   └────────────────────────────────────────────────────────┘
              │
              ▼
   ┌────────────────────────────────────────────────────────┐
   │ PASS 2: Halftone Dot Screening & Plate Transforms      │
   │ - 7 Screen Types: Voronoi, Bayer 8x8, Blue Noise,      │
   │   Line Screen, Stippled, Atkinson, Floyd-Steinberg     │
   │ - 4 Dot Shapes: Circle, Ellipse, Diamond, Square       │
   │ - Simplex / 3D FBM noise surface distortion            │
   │ - Plate transforms: Angle, Skew, Scale, Offset X/Y     │
   │ - Dot gain, softness, and dot hardness response        │
   └────────────────────────────────────────────────────────┘
              │
              ▼
   ┌────────────────────────────────────────────────────────┐
   │ PASS 3: Composite, Overprint, Paper & Imperfections    │
   │ - Physical Overprint: Multiply & Beer-Lambert Law      │
   │ - Total Area Coverage (TAC) ink saturation limiting    │
   │ - Paper substrate: Custom, Newsprint, Cotton, Kraft,   │
   │   Papyrus, Smooth with fiber roughness & lighting      │
   │ - Roller streaks, impression jitter & ink drift        │
   │ - Plate ghosting (previous impression transfer)        │
   │ - Proof Modes: Off, Dot Mask, C, M, Y, K, 4-Up Grid    │
   └────────────────────────────────────────────────────────┘
              │
              ▼
   ┌────────────────────────────────────────────────────────┐
   │ HALO PASS: Micro-Diffusion                             │
   │ - Half-resolution ink bleed and optical fringe         │
   └────────────────────────────────────────────────────────┘
              │
              ▼
[Output to After Effects Composition Buffer]
```

---

## 3. Parameter Group Hierarchy (147 Parameters)

The plugin registers 147 parameters organized into 10 collapsible accordion topics matching the reference plugin:

1. **Top Level**: `Shuffle Seed`, `Press Preset`
2. **Inspect**: `Plate Proof` (Off, Dot Mask, C, M, Y, K, 4-Up Grid), `Magnify Zoom`, `Magnify Center X/Y`, `Plate Enablers (C/M/Y/K)`, `Halo Half-Res`
3. **Machine & Paper**: `Machine Profile`, `Print Resolution`, `Paper Type` (Custom, Newsprint, Cotton Rag, Kraft, Papyrus, Smooth), `Background Mode`, `Spot Ink Kit`, `Working Space`
4. **Color Correction**: `Brightness`, `Contrast`, `Hue`, `Saturation`, `HSV Value`, `Gamma`
5. **Ink Palette**: `Color Mode` (CMYK vs Spot Riso), `Spot Separation` (Greedy vs Balanced Mix), `Line Art Plate/Threshold/Strength`, `Active Plates`, `Coverage`, `Plate Colors`, `Spot Inks 1–4`, `Overprint Mode` (Multiply vs Beer-Lambert), `Translucencies`, `Total Coverage Limit (TAC)`, `Ink Pooling`, `Per-Plate Curves`
6. **Halftone**: `Screen Type` (Voronoi, Bayer, Blue Noise, Line, Stippled, Atkinson, Floyd-Steinberg), `Screen Density`, `Dot Shape` (Circle, Ellipse, Diamond, Square), `Dot Hardness`, `Softness`, `Distortion`, `Dot Stretch`, `Edge Softness`, `Dot Gain`, `Per-Plate Screen Angles & Weights`, `Drum Animation Speed`, `Rich Blacks`
7. **Plate Alignment**: `All-Plate Master Transforms (Offset X/Y, Rotation, Skew, Scale)`, `Per-Plate Displacements (C/M/Y/K)`
8. **Ink Imperfections**: `Ghosting Distance/Opacity`, `Roller Streaks`, `Impression Jitter`, `Paper Reticulation`, `Ink Drift`, `Bleed`
9. **Paper & Grain**: `Paper Roughness`, `Fiber Scale`, `Crumple Depth`, `Aging Tint`, `Lighting Angle`
10. **Master Blend & Output**: `Blend With Original (%)` (default 0.0%), `Master Opacity`, `Output Space`

---

## 4. Installation & Troubleshooting in Adobe After Effects 23.2.1

### Step 1: Copy the Plugin File
Copy `risoprint.aex` (or `resoprint.aex`) into either of the following directories:

- **After Effects Specific Directory**:
  ```
  C:\Program Files\Adobe\Adobe After Effects 2023\Support Files\Plug-ins\risoprint.aex
  ```
- **Shared Adobe MediaCore Directory** (recommended for all Adobe apps):
  ```
  C:\Program Files\Adobe\Common\Plug-ins\7.0\MediaCore\risoprint.aex
  ```

### Step 2: Clear the After Effects Plugin Cache
If After Effects does not list the effect or shows a cached previous version:
1. Close Adobe After Effects 23.2.1 completely.
2. Hold down `Ctrl + Alt + Shift` while launching After Effects until prompted to reset settings, **OR**
3. Navigate to:
   ```
   %APPDATA%\Adobe\After Effects\23.2\
   ```
   and delete `PluginCache64.bin`.
4. Relaunch After Effects. The host will rescan all `.aex` files and detect `risoprint`.

### Step 3: Apply the Effect
In After Effects 23.2.1:
1. Create a composition and select a layer (video or image).
2. Go to the top menu: **Effect > Stylize > risoprint** (Match Name: `RisoPrint_Core`).
3. The effect panel will appear with all 10 collapsible control groups initialized to reference defaults.

---

## 5. Technical Audit & Resolution of Previous Symptoms

1. **Monochrome / Black-and-White Output**:
   - *Cause*: Inserting `Blend With Original` in `ParamsSetup` at index 10 displaced `ID_INK_1` to index 11 (`Enabled 1` checkbox). Reading color structs from checkbox memory produced binary zeros.
   - *Resolution*: Enforced strict 1:1 parameter indexing across all 147 parameters. All ink colors read cleanly from `u.cd.value`.
2. **Left Canvas Shift / Subject Displaced Off-Canvas**:
   - *Cause*: Index 13 queried `Separation 1` (0.38) instead of `Shift X 1` (0.003), shifting plates 38% sideways.
   - *Resolution*: Correct parameter indexing and strict clamping of registration shifts to `[-0.05, +0.05]`.
3. **Source Ghosting**:
   - *Cause*: High nonzero default for `Blend With Original` combined with shifted plates superimposed a duplicate source copy.
   - *Resolution*: Default set strictly to `0.0%` with conditional zero-overhead bypass (`keep <= 0.0001f`).

---

## 6. Build Instructions (Visual Studio 2022 / Windows x64)

Build with CMake 3.20+ and MSVC v143:
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target risoprint --parallel
python ae/verify_build.py dist/risoprint.aex
```

### Automated Code Verification
Run the 6-point audit suite:
```bash
python3 tests/verify_plugin_code.py
```
Validates:
- 1:1 parameter enum alignment (147 params)
- All 7 halftone screen types
- Greedy & Balanced Mix spot decomposition
- Beer-Lambert overprint modeling
- Zero-ghosting bypass
- Plate inspection & 4-Up proof grid

---

## 7. Interactive Web Simulator & Test Harness

Run the local web harness:
```bash
npm run dev
```
Open [http://localhost:3000](http://localhost:3000) to test:
- Live 3-plate and 4-plate risograph rendering with interactive canvas
- All 10 parameter groups and controls
- Split-screen wipe comparison with reference images (`IMG_4003.jpeg` and `IMG_4010.jpeg`)
- Diagnostic root-cause inspector
- One-click repository download bundling all source, headers, PiPL, CMake, and documentation
