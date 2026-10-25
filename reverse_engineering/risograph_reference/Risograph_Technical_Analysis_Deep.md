# Risograph `.rzdb` — Deep Technical Audit (Evidence-Based)

## Scope and honesty note

This audit inspects the actual Rizin project database file, not only the short UI string list. The `.rzdb` is a Rizin analysis database: it records analysis of a Windows x64 binary and also contains recovered source-like strings (including substantial GPU-kernel code). It is not the original authored C++ project tree, and not every original symbol/variable name can be recovered. Conclusions below distinguish observed code from inferred architecture.

## Database inventory

- File bytes: 15,381,059
- Text lines: 195,626
- Analyzed function records: 938
- Sum of function `ninstr` fields: 132,392
- Distinct string records parsed: 21,392
- Unique recovered strings: 18,834
- Unique source-like/kernel-related strings selected for inspection: 54
- Distinct `Riso...()` helper names found in recovered code strings: 58

### Rizin database sections / record counts

- `/` — 2 records
- `/core` — 2 records
- `/core/analysis/blocks` — 20,637 records
- `/core/analysis/callables` — 23,144 records
- `/core/analysis/cc` — 55 records
- `/core/analysis/functions` — 938 records
- `/core/analysis/hints` — 1,077 records
- `/core/analysis/meta` — 23,333 records
- `/core/analysis/meta/spaces` — 2 records
- `/core/analysis/meta/spaces/spaces` — 1 records
- `/core/analysis/types` — 3,906 records
- `/core/analysis/xrefs` — 94,626 records
- `/core/config` — 552 records
- `/core/file` — 3 records
- `/core/flags` — 1 records
- `/core/flags/flags` — 27,254 records
- `/core/flags/spaces` — 2 records
- `/core/flags/spaces/spaces` — 12 records
- `/core/flags/tags` — 10 records
- `/core/seek` — 8 records

## High-confidence findings from recovered code strings

### 1. Plate-coordinate transforms and screen grid

- `RisoApplyPlateXform` applies plate skew and rotation around the image center in pixel-scaled coordinates; it early-outs when both are zero.
- `RisoApplyPlateXformInverse` reverses the rotation/skew transform for mapping grid points back to image UV coordinates.
- `RisoComputeGridPos` transforms UVs, applies per-plate X/Y offsets in pixels, rotates the pixel position by the screen angle, scales by frequency relative to image height, then applies a safe halftone aspect ratio.
- `RisoGridToUV` performs the inverse mapping, including undoing aspect ratio, screen rotation, offsets, and plate transform.
- This directly supports independent CMYK plate rotation/skew/offset controls; it is more than a generic color overlay.

### 2. Voronoi/union-dot halftone

- `RisoVoronoiCell` searches a 3×3 neighborhood around the current integer grid cell, hashes each neighbor to jitter its feature-point center, computes shape-dependent distance, and chooses the closest center.
- `RisoUnionDotInk` repeats a 3×3 neighbor search and takes the maximum ink contribution from the neighboring cells, preventing dots from being evaluated as only one isolated cell.
- `RisoApplyThreshold` adjusts channel coverage, reduces effective intensity as values approach saturation, derives a radius-like threshold from distance, and uses either a hard step or smoothstep based on softness. A saturation floor makes deep shadows become solid instead of leaving square grid gaps.
- The code samples four quarter-pixel offsets in parts of the main path, indicating 2×2 supersampling for smoother dot coverage in that branch.
- A second grid displaced by 20 units is visible in code; this appears to be used for a paired/subsampled dot evaluation, but its precise visual purpose should be confirmed with reference renders.

### 3. Screen types / stochastic modes

- The recovered shader comments explicitly define `VORONOI=1`, `BAYER=2`, `BLUENOISE=3`, `LINE=4`, `STIPPLED=5`, `ATKINSON=6`, `FLOYD=7`.
- The Floyd–Steinberg-like branch uses center hash plus a weighted hash sum with weights 7/16, 3/16, 5/16, 1/16 for E, SW, S, SE, then mixes the center and neighbor threshold estimates 0.4/0.6. This is a deterministic spatial approximation, not conventional stateful scanline error diffusion.
- The Atkinson mode is described in recovered comments as an approximation; it must not be implemented as if the binary proved a full causal error-diffusion buffer.
- Bayer, blue-noise, line-screen and stippled branches are present by named screen type, but exact parity requires tracing each branch and matching its constants.

### 4. Specks, distortion, jitter, halo and ghosting

- `RisoComputeSpecks` early-outs at zero amount, scales noise coordinates by speck size, and uses distinct spatial offsets / z offsets for K/C/M/Y so channels do not share identical noise.
- Grid distortion uses multi-octave FBM/noise to perturb grid coordinates; a recovered formula scales the perturbation by `distortion * 0.30`.
- Impression jitter offsets sampling coordinates with hash-based random values scaled by pixel resolution and jitter magnitude.
- The compositing path can sample blurred CMYK coverage, add halo samples by taking the per-channel maximum after intensity scaling, and add ghost samples similarly using offset UVs and ghost opacity.
- These controls are spatial effects in the render pipeline, not merely decorative UI toggles.

### 5. Coverage / ink response

- Plate coverage is multiplied by ink intensity and per-plate masks; K can additionally be multiplied by a dedicated factor.
- Dot gain is applied as a power transform when above neutral: `coverage = pow(coverage, 1/dotGain)`.
- Per-channel ink curves are applied with inverse curve exponents when values differ from 1.0.
- Roller modulation uses a directional band coordinate and FBM to modulate all four channel coverages.
- Code includes distinct overprint modes, per-plate ink opacity, total-area-coverage limiting, and ink-pooling parameters in the pass interface. Exact equations for every branch need dedicated tracing.

### 6. Compositing, paper and proofing

- The final pass interface includes CMYK ink RGB triplets, paper RGB, paper structure/scale/crumpling/tone, fade and directional fade, color inconsistency, speck controls, dot gain, roller parameters, overprint/opacity/TAC/pooling, halo/ghost, reticulation/show-through, lighting, gamma, proof mode, paper type, color-space mode, per-channel curves, impressions/jitter, soft-proof, negative output, reverse print, ink drift, frame time, background mode, alpha/pixel-layout and render-window parameters.
- Recovered code includes sRGB↔linear conversion helpers, a paper-type branch that reshapes noise coordinates and adds horizontal banding for Kraft/Papyrus, a soft-proof stage, and a 4-up proof layout.
- Proof-mode comments map modes to dot mask, C, M, Y, K and 4-up grid (internal values are zero-based after host popup conversion).

## Pipeline evidence and limits

- Kernel symbols include `RisoPass1Kernel`, `RisoPass2Kernel`, `RisoHaloKernel`, and `RisoPass3Kernel`; OpenCL kernel creation strings are also present.
- The final pass consumes original image, previous pass output and halo blur inputs, consistent with a multi-stage pipeline. The exact stage order and each pass’s complete responsibility should be confirmed by following host-side launch code and buffer wiring, not inferred from names alone.
- The DB has 938 analyzed function records and 94,626 xref records, so literal full reverse engineering requires reviewing call graphs, parameter registration, host callbacks, buffer allocation/launch paths, error/fallback logic, and every kernel branch—not just listing labels.
- There are strings indicating CUDA/OpenCL/Metal/shared-kernel contract and OpenGL fallback, but platform paths should be validated by tracing host dispatch and compiled shader payloads.
- UI labels and recovered shader fragments do not establish exact default values, parameter min/max, hidden licensing behavior, or every host popup mapping. Those require tracing parameter setup/serialization and test renders.

## Next analysis passes required for “literally everything”

1. Recover all registered AE parameters: IDs, names, defaults, min/max, popup item ordering, flags, grouping, and version migration.
2. Trace `EffectMain`, parameter setup, sequence/global setup, render dispatch, smart-render callbacks, and any legacy render path.
3. Trace every GPU kernel’s arguments and exact launch order / buffers; document fallback paths and error handling.
4. Enumerate all helper functions and create a call-graph map, focusing on each `Riso...` helper plus large anonymous functions.
5. Map every user-visible control to the exact variable(s), transforms, clamping and branch that consume it.
6. Compare renders for standard CMYK, Spot Riso, each screen type, plate proof, paper types, negative/reverse, and edge cases; without reference renders, exact visual equivalence cannot be claimed.

## Guidance for the RisoPrint implementation AI

Treat this file as a technical map, not as proof that every feature is already implemented. Keep RisoPrint’s existing AE SDK/build structure intact. Implement an independently authored equivalent from documented behavior. First preserve a minimal working AEX and verify it appears in After Effects 23.2.1 x64; then add features in stages and test after each stage. Do not copy the compiled proprietary binary into the build as executable code, and do not claim a successful build means AE loaded the plugin.