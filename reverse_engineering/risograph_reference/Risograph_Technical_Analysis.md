# Risograph.aex.rzdb — Technical Analysis for RisoPrint

## Scope and confidence
This report is based on the Rizin database (`Risograph.aex.rzdb`) and extracted strings / embedded GPU-kernel text. It describes observable names and logic present in the database. It is not the original vendor C++ project, and exact visual parity still needs render comparisons.

## Confirmed processing architecture
The database contains embedded GPU kernel text and kernel creation names for four kernels: `RisoPass1Kernel`, `RisoPass2Kernel`, `RisoHaloKernel`, and `RisoPass3Kernel`. Function signatures and code fragments support this working pipeline:

1. **Pass 1 — color preparation and plate decomposition.** Inputs include brightness, contrast, hue, saturation, value delta, gamma, dark-tone enrichment, color mode, spot-plate count, spot-separation mode, and line-art settings. In CMYK mode, the visible code computes C/M/Y from RGB, extracts K as the minimum of the three subtractive channels, normalizes C/M/Y by `1-K` when possible, and mixes the normalized values with the original subtractive channels according to `enrichDarkTones`. In Spot Riso mode, it decomposes the target color against the selected ink axes.
2. **Spot separation methods are materially different.** `Greedy (Legacy)` projects onto each ink axis in sequence and subtracts that contribution from the remaining target, clamping residual channels at zero. `Balanced Mix` iteratively updates ink coefficients over four sweeps, with coordinate updates across active plates. It is not just a UI label swap.
3. **Line-art plate routing.** A helper chooses the target channel based on the configured line-art plate and color mode, then raises that channel's coverage to at least the line mask (`max(existing coverage, lineMask)`). The user-facing controls include Line Art Plate, Threshold, and Strength.
4. **Pass 2 — screen/halftone generation.** Its signature includes scale, dot shape, halftone intensity, softness, distortion, aspect, dot smoothness, screen type, specks amount/size, quality, drum animation, per-plate angles and offsets, rotations, and skews. This supports a separate screen/plate-generation stage, rather than a single generic posterize filter.
5. **Halo stage.** A separate `RisoHaloKernel` exists and Pass 3 accepts an `inHaloBlur` buffer plus halo radius/intensity. This supports a dedicated halo/blur contribution, although its exact invocation order and all internal calculations should be confirmed from the host dispatch code before asserting the exact sequence.
6. **Pass 3 — print/material simulation and composite.** The parameter signature includes per-ink RGB values and opacity, ink intensity, K multiplier, paper color/structure/scale/crumpling/tone, ink fade, color inconsistency, specks, dot gain, roller amount/frequency/angle, overprint mode, total-area coverage (TAC) limit, ink pooling, halo, ghost offsets/opacity, reticulation/scale, show-through, drum animation, plate masks, lighting, output gamma, proof mode, paper type, per-plate curves, impressions/jitter, soft proof, negative/reverse print, and ink drift. These are distinct material/output controls rather than names only.

## Confirmed overprint behavior
- `Multiply` is the legacy mode and is described in embedded comments as order-dependent.
- `Beer-Lambert` uses an exponential transmittance/density approach (`exp(-...)`) and applies per-ink opacity/translucency. The embedded comments explicitly clarify that Black Boost multiplies K coverage once before the overprint calculation; it is not intended to be applied twice.
- `Total Coverage Limit` clamps the combined plate coverage. Ink pooling and roller modulation alter plate coverage before compositing; a code fragment uses FBM noise to modulate coverage along a roller-frequency coordinate.

## UI/control families found
- Input/color: Brightness, Contrast, Hue, Saturation, HSV Value, Gamma, dark-tone enrichment.
- Screens: Screen Density, Dot Shape (Circle/Ellipse/Diamond/Square), softness, distortion, aspect, smoothness, screen type, specks, quality, drum animation.
- Plates: per-plate angle, offset X/Y, rotation, skew, weight/curve, masks and inspection/proof modes.
- Ink/composite: ink colors, intensity, Black Boost, translucency, overprint, TAC, pooling, dot gain, fade, ghosting, reticulation, show-through, drift.
- Paper/finish: paper color and structure, scale, crumpling, tone, lighting, soft proof profiles, paper types.
- Spot workflow: 2–4 active inks, named ink palettes/kits, Greedy vs Balanced Mix, spot separation.
- Output/diagnostics: negative/reverse print, line-art plate, magnifier modes, print resolution presets, plate proof modes, machine profile, impressions and jitter.

## Guidance for implementing RisoPrint
1. Preserve RisoPrint's existing project/build layout. Do not rename build targets, entry points, PiPL resources, or plugin identifiers as part of the effect port.
2. Implement a minimal discoverable AE effect first and verify it loads in the target AE version before adding GPU processing.
3. Add controls in stages: input/color preparation → plate decomposition → screen generation → ink/paper composite → proof/inspection tools.
4. Keep CMYK and Spot Riso as separate modes. Implement Greedy and Balanced Mix separately; do not fake them as aliases.
5. Make CPU fallback or a clearly handled GPU-unavailable path if the target AE/SDK and available hardware cannot run the GPU path. Do not silently produce a blank output.
6. Validate using fixed test images and matching settings. Compare plate masks, individual separations, dot geometry, overprint behavior, and final composite independently.
7. Treat undocumented defaults, parameter ranges, exact kernel dispatch order, and visual details as unknown until verified. Use the original effect only as a behavioral reference; do not paste a proprietary binary or a byte-dump in place of source code.

## What this analysis does not establish yet
- Exact default values and legal ranges for every parameter.
- The complete host-side order and synchronization of all GPU kernels.
- Pixel-exact parity for every screen type, paper texture, and ink preset.
- How the plugin stores presets or handles all AE render contexts.

## Files in this analysis set
- `Risograph.aex.rzdb`: Rizin analysis database for the compiled plugin.
- `Risograph_Extracted_Strings.txt`: extracted strings and embedded code fragments.
- `dump.c`: byte-array representation/dump; not the original maintainable C++ source.
