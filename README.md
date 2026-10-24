# risoprint — native After Effects Risograph Effect

risoprint is a native Windows x64 `.aex` effect for Adobe After Effects. Its build/registration skeleton is based on the known-good `Dithering_MAJEED_build_ready` repository structure, while the render engine is new and dedicated to the three-plate risograph look.

## Engine

The effect is modeled as three overlapping ink plates printed onto paper:

- color-aware plate separation
- per-plate Separation amount
- per-plate Shift X / Shift Y registration drift
- paper color
- ink contrast
- grain Scale
- paper/ink Opacity texture response
- source Softness
- global Contrast
- Intensity
- three editable inks
- deterministic animated print grain

The defaults match the supplied reference settings screenshots:

- Paper: `#FFFFFF`
- Ink contrast: `-0.33`
- Scale: `0.21`
- Opacity: `0.65`
- Softness: `0.50`
- Contrast: `1.58`
- Ink 1: `#50D9F1`, Separation `0.38`, Shift X `0.003`, Shift Y `0.002`
- Ink 2: `#BF7700`, Separation `0.64`, Shift X `0.000`, Shift Y `-0.001`
- Ink 3: `#2900FF`, Separation `0.39`, Shift X `0.000`, Shift Y `-0.003`

## Build

GitHub Actions uses Windows 2022 + MSVC x64 + CMake, matching the proven reference approach. The final artifact is `risoprint.aex`.

Target After Effects: Windows x64, including the user's AE 2023 / 23.x environment.


## Current build/runtime fixes
- Plugin display/match name: `risoprint`
- PiPL `AE_Effect_Version` uses the Adobe `PF_VERSION` encoding required by the reference build (`01 06 08 00` for code version 1.0.0 RELEASE build 1).
- CMake has one real plugin target (`risoprint`) and no same-name custom target, preventing MSBuild circular dependencies.
- Exposed controls: Intensity, Blend, Show Source, Paper, Ink Contrast, Scale, Opacity, Softness, Contrast, and 3 complete ink plates.
- Each ink plate exposes Enabled, Ink Color, Separation, Shift X, Shift Y.
- The engine reads every exposed control during Smart Render.
