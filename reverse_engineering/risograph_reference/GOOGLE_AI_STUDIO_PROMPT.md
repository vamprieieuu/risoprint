# Google AI Studio Prompt — RisoPrint / Risograph Behavioral Reimplementation

You are working inside the existing **RisoPrint** native Adobe After Effects effect-plugin repository. The goal is to independently reimplement the observable behavior and useful functionality of the reference effect **Risograph** inside RisoPrint, because the original plugin does not run reliably on the user's computer (possibly due to performance or compatibility constraints). Target environment: Windows 11 x64, Adobe After Effects 23.2.1, Intel UHD Graphics 620, 8 GB RAM. Favor robust CPU-compatible behavior and efficient processing; do not require a powerful GPU.

## Reference materials in this repository

Read every file in `reverse_engineering/risograph_reference/` before coding:
- `Risograph.aex.rzdb` — the original analysis database supplied by the user. Inspect it with suitable Rizin/Rizin Database tooling if available. Do not assume its binary format is plain source code.
- `Risograph_Extracted_Strings.txt` — extracted strings / labels.
- `Risograph_Technical_Analysis.md` and `Risograph_Technical_Analysis_Deep.md` — preliminary analyses; treat claims as hypotheses unless confirmed against the artifacts.
- `Risograph_DB_Embedded_Code_Strings.txt` — selected code-like strings.
- `Risograph_RisoFunction_Names.txt` — function/symbol names found in the database.

The user will add screenshots of the original plugin's complete UI, tabs, parameter controls, dropdowns, defaults, and example outputs to `reverse_engineering/risograph_reference/images/`. When those images exist, inspect **every image** and use them as visual evidence for UI labels, grouping, defaults, parameter ranges, and appearance. Do not claim to have inspected screenshots that are not present.

## Main objective

Build one After Effects plugin (`.aex`) with the RisoPrint identity and multiple controls/effects in a single plugin, recreating as much of the reference's *observable functionality* as can be established. Do not merely create controls that do nothing. Each implemented control must affect rendering in a meaningful, testable way. Do not claim exact equivalence where evidence is insufficient; keep an explicit list of unknowns.

## Required workflow — do these in order

1. **Audit the current repository before editing.**
   - Read the root `README.md`, `CMakeLists.txt`, GitHub Actions workflow, AE entry source, core source/header files, PiPL resources, `.def` files, version resources, and build verification scripts.
   - Identify the actual plugin target, exported entry point, PiPL registration name/category, architecture, output path, and any duplicate/legacy source files.
   - Preserve the current known build structure and explicit source lists. Do not glob source files, rename the target, change the plugin identity or restructure build files casually.
   - Make a backup or use a focused diff before substantial edits. Avoid touching the bundled After Effects SDK unless a specific compile error requires it.

2. **Reverse-engineer the evidence, not just the names.**
   - Inspect the `.rzdb` and associated text artifacts for metadata, function names, strings, references, constants, parameter labels, likely parameter IDs/defaults/ranges, shader/kernel text, and relationships between parameters and code.
   - Search for registration code and render logic evidence. Correlate UI screenshots with extracted strings and symbols.
   - Separate findings into: CONFIRMED, STRONGLY INDICATED, and UNKNOWN. Do not invent defaults, ranges, algorithm order, or exact kernel behavior when they cannot be recovered.
   - `RisoPass1Kernel`, `RisoPass2Kernel`, `RisoPass3Kernel`, and `RisoHaloKernel` are observed names. Do not assume their precise roles or execution order until verified.

3. **Map every visible control to behavior.**
   Use the reference labels and screenshots to create a complete control inventory. At minimum, investigate these feature families when evidence supports them:
   - Core tone/color: Saturation, HSV Value, Gamma, Halftone, Screen Density, Dot Shape (Circle/Ellipse/Diamond/Square), Dot Hardness, Softness, Distortion, Dot Stretch, Edge Softness.
   - CMYK plate angles/weights, plate alignment, per-plate X/Y offsets, rotation/skew, translucency, ink palette, coverage, black boost/rich blacks, total coverage limit, ink pooling, plate curves and overprint mode.
   - CMYK and Spot Riso modes, active plates, named spot inks and ink kits.
   - Screen/dither methods including Voronoi, Bayer 8x8, blue noise, line screen, stippled, Atkinson sparse, and Floyd–Steinberg clustered, where supported by evidence.
   - Paper/grain, paper types/colors, reticulation, ink drift, impression jitter, machine profiles, soft proof, negative/reverse print, line-art plate/threshold/strength.
   - Inspect/magnify modes, plate isolation, print resolution, plate proof outputs, and all-plate transforms.
   This list is a starting inventory, not permission to invent unsupported features. Match screenshots and source evidence.

4. **Implement incrementally and correctly.**
   - First ensure the current plugin still builds and is discoverable by AE 23.2.1 before implementing major rendering changes.
   - Keep the plugin as one `.aex` with multiple controls in the effect UI; do not split features into separate plugins.
   - Implement a minimal real render path first, then add rendering stages incrementally. A staged CPU implementation is acceptable and may be preferable for compatibility; only use GPU code if the existing codebase supports it safely.
   - Use proper AE parameter setup, stable parameter IDs, valid pixel-format handling, correct row-byte/stride handling, bounds checks, and safe memory management. Handle 8/16/32 bpc only if the existing architecture supports them; otherwise explicitly support and report the supported formats.
   - Avoid per-pixel allocations and avoid expensive full-frame work when parameters do not require it. Cache reusable lookup tables where safe. Do not block AE's UI thread with long-running setup.
   - Keep parameter names, defaults and ranges consistent with confirmed screenshots/evidence. Group controls logically and give each real behavior.
   - Preserve build workflow and target output path. Do not say the build succeeded unless you actually ran it and inspected the artifact.

5. **Use the user's screenshots and reference outputs as tests.**
   - When reference images are available, derive reproducible test cases from them.
   - Create a mapping table: `reference control → evidence → RisoPrint parameter → implementation function → test`.
   - Where reference output images exist, compare outputs visually and record known differences. Do not pretend to achieve pixel-identical results without tests.

6. **Verify the result.**
   - Run the repository's existing build and verification workflow if the environment permits.
   - Confirm that the resulting `.aex` exists, is x64, has the expected exports/PiPL registration, and is placed at the expected output path.
   - Add a practical troubleshooting checklist for AE 23.2.1 plugin discovery: correct MediaCore/Plug-ins path, Windows block/unblock if relevant, matching architecture, duplicate plugin versions, AE cache/restart, and PiPL/entry-point validation.
   - If you cannot execute a Windows build or launch AE in your environment, say so explicitly and provide exact next steps. Never claim runtime testing you did not perform.

## Repository safety and constraints

- Do not delete existing source files or SDK files just because they look unused. First determine whether build scripts reference them.
- Keep changes focused and explain each changed file.
- Do not replace the build system or target names merely to simplify your work.
- Do not copy the original compiled plugin's binary code into the new plugin, patch the original binary, or represent the `.rzdb` database as original C++ source. Use the database and extracted artifacts as analysis evidence to create an independent implementation of the observed functionality.
- Do not add placeholder controls that have no effect. If a control is not implemented yet, mark it as TODO in the inventory and expose it only if clearly labeled as not implemented.
- Prefer performance and compatibility with the user's modest system over unnecessarily complex GPU dependencies.

## Required deliverables

1. A working, single RisoPrint `.aex` containing the implemented functionality.
2. Updated source code and build configuration, with the existing build structure preserved.
3. `reverse_engineering/risograph_reference/CONTROL_MAPPING.md` — every known control, evidence level, implementation location, and test status.
4. `reverse_engineering/risograph_reference/REVERSE_ENGINEERING_FINDINGS.md` — confirmed facts, hypotheses, unknowns, and evidence references.
5. `reverse_engineering/risograph_reference/IMPLEMENTATION_STATUS.md` — implemented, partial, and unimplemented features, with honest build/runtime status.
6. A concise `CHANGELOG.md` entry and exact build/install/test instructions for After Effects 23.2.1.
7. Keep the original `.rzdb`, analyses, and user-added reference screenshots in the repository without modifying their contents.

Start by auditing the repository and the reference files. Then provide a short plan based on what is actually present, and proceed with implementation. Do not stop after writing a plan or just renaming UI labels; carry out the code changes and verification.
