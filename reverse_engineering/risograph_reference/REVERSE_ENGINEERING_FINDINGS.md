# Risograph Reference — Reverse Engineering Findings

## Executive Summary
This document records the empirical reverse engineering findings derived from `Risograph.aex.rzdb` (the Rizin binary analysis database of the compiled Windows x64 After Effects effect plugin), `Risograph_Extracted_Strings.txt`, `Risograph_DB_Embedded_Code_Strings.txt`, `Risograph_RisoFunction_Names.txt`, and 28 high-resolution screenshots of the original plugin running inside Adobe After Effects 2023 (v23.2.1) on Windows 11.

Each finding is classified into one of three strict evidentiary confidence levels:
- **CONFIRMED**: Backed by direct visual evidence from UI screenshots or verified code/decompilation strings in the database.
- **STRONGLY INDICATED**: Directly suggested by function symbols, cross-references, and parameter layouts, but where minor internal branch constants or host synchronization details may vary.
- **UNKNOWN**: Elements not determinable from the offline database or visual screenshots alone (e.g. GPU host buffer fence ordering, license server handshakes).

---

## 1. Confirmed Processing Architecture

The embedded shader text and exported symbols explicitly define a 3-pass (+ halo) pipeline:

### Pass 1: Color Preparation, Working Space & Plate Decomposition (`RisoPass1Kernel`)
- **Working Space**: Converts input RGB to linear working space (`RisoSrgbToLinear3`) when `Working Space` is set to "Linearize sRGB" (mode 2), or leaves as sRGB when set to "Passthrough" (mode 1).
- **Color Correction**:
  - `Brightness` (-100 to +100, default 0.0)
  - `Contrast` (-100 to +100, default 0.0)
  - `Hue` (-180° to +180°, default 0.0°)
  - `Saturation` (-100 to +100, default 0.0)
  - `HSV Value` (-100 to +100, default 0.0)
  - `Gamma` (0.1 to 3.0, default 1.00)
- **Line Art Extraction (`RisoLineArtMask`)**:
  - Computes center luminance (`RisoLuma3`) and 4-neighbor edge contrast against left, right, up, down pixels.
  - Formula:
    ```c
    float darkMask = RisoClamp01((threshold - centerLuma) * 4.0f);
    float edgeStart = 0.18f - threshold * 0.10f;
    float edgeMask = RisoClamp01((edgeContrast - edgeStart) * 5.0f);
    float lineMask = RisoClamp01(fmax(darkMask, edgeMask) * strength);
    ```
  - Routes the line mask to the selected plate via `RisoApplyLineArt`, raising that channel's coverage to `fmax(existingCoverage, lineMask)`.
- **Plate Separation Modes**:
  - **CMYK Mode**:
    Subtractive conversion: $C_{raw} = 1 - R$, $M_{raw} = 1 - G$, $Y_{raw} = 1 - B$.
    Key extraction: $K = \min(C_{raw}, M_{raw}, Y_{raw})$.
    Normalization by $(1 - K)$ when $K < 1.0$.
    Dark tone enrichment: mixes normalized subtractive components with original values using `enrichDarkTones`.
  - **Spot Riso Mode**:
    Active plate count (2 to 4 plates).
    - `Greedy (Legacy)` (`RisoSpotGreedyDecompose`): Sequentially projects the target subtractive color onto spot ink vectors $a_0, a_1, a_2, a_3$ and subtracts the projected amount from the residual target, clamping at zero.
    - `Balanced Mix` (`RisoSpotBalancedDecompose`): Performs 4 sweeps of coordinate descent updates (`RisoSpotCoordUpdate`) over active plates to find an optimal non-negative linear combination that matches the target.

### Pass 2: Halftone & Screen Generation (`RisoPass2Kernel`)
- **Plate Coordinates & Registration**:
  - Plate skew and rotation: `RisoApplyPlateXform` rotates around the center $(0.5, 0.5)$ in pixel coordinates and applies shear: $q_x += q_y \times \text{skew}$.
  - Grid position: `RisoComputeGridPos` scales UV by `freq` relative to height, rotates by screen angle, and incorporates per-plate offsets $(offX, offY)$ and all-plate global transforms.
- **Halftone Screens (Confirmed Screen Types 1–7)**:
  1. `Voronoi` (type 1): 3×3 neighbor jittered grid cell search (`RisoVoronoiCell`, `RisoUnionDotInk`) evaluating shapes (Circle, Ellipse, Diamond, Square). Evaluates 2×2 supersampling offsets $(\pm 0.25, \pm 0.25)$ and smoothstep thresholding.
  2. `Bayer 8x8` (type 2): Ordered dither threshold matrix with normalized cell step $bayerStep = altCellPx / 8.0$.
  3. `Blue Noise` (type 3): Procedural high-frequency void-and-cluster blue noise hash.
  4. `Line Screen` (type 4): Periodic sine/sawtooth line raster along the plate angle.
  5. `Stippled` (type 5): High-frequency stochastic distribution simulating sand grain / spray stipple.
  6. `Atkinson (sparse)` (type 6): Deterministic spatial error distribution approximation.
  7. `Floyd-Steinberg (clustered)` (type 7): Deterministic spatial approximation using weighted neighbor hashes: $h_1 = h(E) \cdot \frac{7}{16} + h(SW) \cdot \frac{3}{16} + h(S) \cdot \frac{5}{16} + h(SE) \cdot \frac{1}{16}$, then threshold mixed $h_0 \cdot 0.4 + h_1 \cdot 0.6$.
- **Grid Distortion**: Perturbs grid positions using 3D FBM noise scaled by `distortion * 0.30f`.
- **Specks**: Computes independent multi-octave noise specks per plate ($C, M, Y, K$) with drum animation z-offset.

### Pass 3: Material Simulation, Wear, Paper, Overprint & Composite (`RisoPass3Kernel`)
- **Paper Stock & Crumpling**:
  - 2-octave FBM crumpling coordinates (`RisoApplyProofCrumpleCoordinate`).
  - Paper textures for Newsprint, Cotton Rag, Kraft (horizontal banding), Papyrus, Smooth Coated.
  - Paper tone and aging color mix.
- **Plate Wear & Mechanical Imperfections**:
  - Roller streaks: directional band coordinate $coord = uv.y \cos\theta - uv.x \sin\theta$ modulated with FBM noise.
  - Dot gain: power curve `pow(coverage, 1.0 / dotGain)`.
  - Individual plate curves: `pow(coverage, 1.0 / inkCurve)`.
  - Halo stage: samples blurred coverage with `haloRadius` and `haloStrength`.
  - Ghosting: offset UV lookup with `ghostOffsetX`, `ghostOffsetY`, and `ghostOpacity`.
  - Sun bleach: directional ink fade along `bleachDirection`.
  - Ink reticulation: Voronoi-like cell aggregation.
  - Impression jitter and ink drift.
- **Overprint Physics**:
  - `Multiply`: legacy subtractive order-dependent multiplication ($base \times ink$).
  - `Beer-Lambert`: physical exponential absorption $T = \exp(-\sum \alpha_i \cdot \text{density}_i \cdot \text{translucency}_i)$.
  - `Total Area Coverage (TAC)`: clamps aggregate plate coverage to `Total Coverage Limit (%)` (100% to 400%, default 300%).
- **Lighting & Finish**:
  - Directional artificial lamp (`RisoArtificialLamp`) using surface normals derived from paper structure and crumpling gradients.
  - Ambient light and 3-point studio lighting.
  - Output gamma correction (default 1.30).
  - Reverse print (white ink on dark background).
  - Proof modes: Off (Composite), Dot Mask Only, Cyan Separation, Magenta Separation, Yellow Separation, Black Separation, 4-Up Proof Grid with crosshair lines.

---

## 2. Evidentiary Audit Checklist

| Item | Evidence Level | Evidence Source |
|---|---|---|
| Complete Parameter Tree & Labels | CONFIRMED | AE 2023 Screenshots `AfterFX_*.png` (28 images) |
| Parameter Default Values & Precisions | CONFIRMED | Screenshots `AfterFX_2FTacdKl01.png` - `AfterFX_xkKxHij8TL.png` |
| Screen Type IDs (1 to 7) | CONFIRMED | `Risograph_DB_Embedded_Code_Strings.txt` lines 17, 30 |
| Voronoi 3x3 Union Dot algorithm | CONFIRMED | `Risograph_DB_Embedded_Code_Strings.txt` string 3 & string 12 |
| Balanced Mix 4-sweep coordinate descent | CONFIRMED | `Risograph_DB_Embedded_Code_Strings.txt` string 1 |
| Beer-Lambert overprint calculation | CONFIRMED | `Risograph_Technical_Analysis.md` line 18 & shader strings |
| Paper Kraft horizontal banding formula | STRONGLY INDICATED | Decompiled paper type switch branch in .text section |
| GPU buffer synchronization across passes | UNKNOWN | Host-side OpenCL/CUDA command queue synchronization not preserved in string dump |
| Exact vendor licensing check | UNKNOWN | Proprietary string hashes stripped / bypass implemented |

---

## 3. Conclusions for RisoPrint Implementation
1. **Single Plugin Target**: RisoPrint must remain a single `.aex` plugin with the established identity `risoprint` and category `risoprint`.
2. **Robust CPU Execution**: Given the target environment (Intel UHD 620, 8 GB RAM, Windows 11), a clean, high-performance C++ CPU pipeline handles all bit depths (8-bit, 16-bit, 32-bit float) deterministically without GPU memory crashes or OpenCL driver mismatches.
3. **No Phantom Controls**: Every parameter present in the control mapping connects directly to an active algorithmic calculation in the rendering pipeline.
