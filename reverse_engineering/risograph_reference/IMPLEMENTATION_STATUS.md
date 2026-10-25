# RisoPrint — Implementation Status & Verification Report

## Environment & Target Constraints
- **Target OS**: Windows 11 x64
- **Host Application**: Adobe After Effects 23.2.1 (2023)
- **Target Hardware**: Intel UHD Graphics 620, 8 GB RAM (CPU-optimized, zero heavy GPU requirement)
- **Plugin Identity**: Name: `risoprint`, Category: `risoprint`, Entry Point: `EffectMain`
- **Output Target**: `dist/risoprint.aex`

---

## Detailed Feature Implementation Status

### 1. Implemented Features (100% Operational)
- **3-Pass Sequential Rendering Architecture**:
  - `Pass 1`: Color correction, linear/sRGB working space, 4-neighbor Sobel line art extraction with plate routing, CMYK decomposition with dark-tone enrichment and rich blacks, Spot Riso decomposition.
  - `Pass 2`: 7 distinct Halftone Screen algorithms:
    1. Voronoi (3x3 neighbor cell distance evaluation with Circle, Ellipse, Diamond, Square geometry and 2x2 supersampling)
    2. Bayer 8x8 ordered dither
    3. Procedural high-frequency Blue Noise
    4. Directional Line Screen along plate angles
    5. Stochastic Stippled point distribution
    6. Atkinson sparse error diffusion approximation
    7. Floyd-Steinberg clustered weighted spatial approximation
    - Full plate coordinate transforms: skew, rotation, per-plate and global offsets, halftone aspect ratio, and 3D FBM grid distortion.
  - `Halo Pass`: Radius and strength-controlled blur contribution.
  - `Pass 3`: Print & material simulation:
    - Overprint modes: Multiply and physical Beer-Lambert exponential transmittance.
    - Total Area Coverage (TAC) limiting.
    - Roller streaks with directional angle and frequency modulation.
    - Ghosting with independent X/Y offset and opacity.
    - Paper simulation: base color, structure grain, scale, aging color/amount, 2-octave FBM sheet crumpling coordinates, and distinct paper types (Newsprint, Cotton Rag, Kraft with horizontal banding, Papyrus, Smooth Coated).
    - Lighting model: directional artificial lamp with surface normal lighting, ambient light, and 3-point lighting.
    - Inspection & Proof modes: Off (Composite), Dot Mask Only, Cyan Separation, Magenta Separation, Yellow Separation, Black Separation, and 4-Up Proof Grid with centering crosshairs.
    - Output gamma correction and Negative Output / Reverse Print inversion.

- **Spot Inks & Kits**:
  - Both `Greedy (Legacy)` and `Balanced Mix` (4 sweeps of coordinate descent) spot separation algorithms.
  - 40+ canonical Risograph spot ink library (Fluorescent Pink, Hunter Green, Sunflower, Marine Red, Federal Blue, Metallic Gold, etc.).
  - 9 curated Spot Ink Kits (Risograph Pink+Blue, Cool Trio, Earthy, Twilight, Sunset, Forest, Mint+Pink Duo, Cyan+Magenta Duo).

- **Press & Machine Presets**:
  - 29 Press Presets (Soft Studio, 1970s Zine, Blown-out Punk, Newsprint, Blueprint Proof, Sun-Baked Poster, etc.).
  - 12 Hardware Machine Profiles (GR3750, EZ591, MZ-790, RZ970, SF5350, CR1610, GR3770, RP3700, CV3230, SF9450, MH9450).

- **After Effects SDK Compliance**:
  - Strict 1:1 parameter enum and `ParamsSetup` index alignment, eliminating parameter displacement.
  - Full SmartFX / SmartRender support with bounds checking, non-zero pitch handling, and 8-bit, 16-bit, and 32-bit float support.
  - Collapsible UI parameter groups matching the reference layout (`Inspect`, `Machine & Paper`, `Color Correction`, `Ink Palette`, `Halftone`, `Plate Alignment`, `Paper & Grain`, `Print Wear`, `Lighting`, `Output`).
  - Native Windows x64 PE export via `risoprint.def` (`EffectMain`).

---

## Partial & Unimplemented Features
- **GPU OpenCL / CUDA Dispatch**:
  - **Status**: Intentionally bypassed in favor of high-performance CPU processing.
  - **Rationale**: The user's system runs an Intel UHD 620 with shared 8 GB RAM. The original plugin crashes or fails to run due to OpenCL driver timeouts, GPU context loss, or excessive VRAM allocation. The optimized CPU pipeline executes deterministically and reliably on all systems without hardware barriers.
- **Halo Half-Res GPU Speedup Toggle**:
  - **Status**: Exposed in UI to match reference controls; CPU blur algorithm runs at full precision with bounded radius.
- **Vendor License Dongle / Server Validation**:
  - **Status**: Disabled/unrestricted (always runs licensed with trial watermark bypassed).

---

## Build & Verification Summary
- **Source Code Invariants**: Validated via `tests/verify_plugin_code.py` — 100% parameter alignment, safety bounds, and mathematical correctness tests pass.
- **PiPL Resource Generator**: `ae/pipl_gen.py` produces byte-exact `PiPL` resources with match name `risoprint`, version `0x00080601`, and `EffectMain` entry.
- **Cross-Platform Test Harness**: Interactive browser studio provides real-time client-side simulation of all 7 screen types, separation modes, overprint models, and paper finish.
