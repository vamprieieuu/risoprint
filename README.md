# ResoPrint — native After Effects Risograph Effect

ResoPrint is a native Windows x64 `.aex` effect for Adobe After Effects. Its build/registration skeleton is based on the known-good `Dithering_MAJEED_build_ready` repository structure, while the render engine is new and dedicated to the three-plate risograph look.

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

GitHub Actions uses Windows 2022 + MSVC x64 + CMake, matching the proven reference approach. The final artifact is `ResoPrint.aex`.

Target After Effects: Windows x64, including the user's AE 2023 / 23.x environment.
