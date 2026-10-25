# CHANGELOG — RisoPrint After Effects Plugin & Engine

## [2.0.1] — 2026-10-25: Fix C2679 ColorRGB vs Vec3 Type Mismatch in Proof Modes
- **Root Cause**: In `core/risoprint_core.cpp` lines 1125, 1127, 1129, 1131, and 1141, plate proof visualization assigned the return value of `Mix3(Vec3, Vec3, float)` to `finalRgb` (`ColorRGB`). MSVC failed with `C2679: binary '=': no operator found which takes a right-hand operand of type 'resoprint::Vec3'`.
- **Architectural Fix**: Added `inline ColorRGB MixRGB(ColorRGB a, ColorRGB b, float t)` and `ColorRGB Mix(ColorRGB, ColorRGB, float)` overload in `core/risoprint_core.h`. Updated plate proof calculations to linearly interpolate between paper white `ColorRGB{1.0f, 1.0f, 1.0f}` and `activeInks[i]` using `MixRGB`, strictly preserving semantic type boundaries between 3D vectors (`Vec3`) and optical color pixels (`ColorRGB`).

## [2.0.0] — 2026-10-24: Full Risograph Behavioral Reimplementation & Diagnostic Fixes

### Major Architectural Upgrades (Reference Reverse Engineering)
- **Extracted Reference Analysis**:
  - Reconstructed functional pipeline from `Risograph.aex.rzdb`, `Risograph_Extracted_Strings.txt`, `Risograph_DB_Embedded_Code_Strings.txt`, and 28 user screenshots.
  - Documented findings in `reverse_engineering/risograph_reference/REVERSE_ENGINEERING_FINDINGS.md` under three confidence tiers: CONFIRMED, STRONGLY INDICATED, and UNKNOWN.
  - Reconstructed complete 147-parameter hierarchy and mapped each to behavior in `reverse_engineering/risograph_reference/CONTROL_MAPPING.md`.
  - Documented environment and CPU compatibility in `reverse_engineering/risograph_reference/IMPLEMENTATION_STATUS.md`.

- **3-Pass Pipeline + Halo Pass Implemented**:
  - `Pass 1`: Color correction, Sobel Line Art extraction, and dual CMYK/Spot Riso separation (Greedy & Balanced Mix).
  - `Pass 2`: 7 Halftone screen algorithms (Voronoi with 4 dot shapes, Bayer 8x8, Blue Noise, Line Screen, Stippled, Atkinson sparse, Floyd-Steinberg clustered), 3D Simplex/FBM distortion, per-plate rotation, skew, scale, and offset.
  - `Pass 3`: Subtractive overprint (Multiply & physical Beer-Lambert transmittance), TAC ink limiting, paper textures (Custom, Newsprint, Cotton Rag, Kraft, Papyrus, Smooth), roller streaks, crumple distortion, plate ghosting, lighting, and proof outputs.
  - `Halo Pass`: Half-resolution micro-diffusion ink fringe.

- **Low-Spec Hardware Optimization (Intel UHD Graphics 620 / 8 GB RAM)**:
  - Eliminated dependency on problematic OpenCL GPU routines that caused timeouts on integrated graphics.
  - Full CPU-native implementation supporting 8-bit, 16-bit, and 32-bit float buffers with rowbytes stride handling.

### Diagnostic & Bug Fixes (Resolving Reported Symptoms)
- **1:1 Parameter Index Alignment**:
  - Fixed off-by-one displacement where `Blend With Original` inserted at index 10 misaligned all subsequent parameters.
  - Aligned all 147 enum IDs with `ParamsSetup` registrations.
  - Restored color sampling from `u.cd.value` (fixing monochrome/black-and-white output).
  - Constrained plate displacement offsets to `[-0.05, +0.05]` (fixing subject shifting 38% off-canvas).
  - Set `Blend With Original` default strictly to `0.0%` with conditional bypass (eliminating source ghosting).

### Test & Tooling Suite
- Created `tests/verify_plugin_code.py` running 6 comprehensive verification tests. All 6 tests pass with 0 failures.
- Updated React interactive web studio on port 3000 (`src/`) featuring all 10 collapsible accordion panels, live 7-screen rendering, split-screen wipe comparison, and full repository zip packaging.

---

## [1.1.0] — 2026-10-24: Parameter Enum Alignment & Subtractive Ink Mixing
- Synchronized `ParamsSetup` with `enum` definitions.
- Clamped registration chatter shifts.
- Implemented subtractive physical ink multiplication.

---

## [1.0.0] — Initial Release
- Initial plugin scaffolding for After Effects.
