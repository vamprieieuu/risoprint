# Implementation brief — risoprint for After Effects

You are given the current source repository and the original visual references. Your task is to debug and implement the intended effect in the repository, not to give generic advice.

## Goal / intended visual result
Create a convincing **full-colour risograph / multi-ink print effect** based on the supplied before-and-after images and the settings screenshots. Preserve the recognizable subject and its fine details. The result should retain the original dark red/burgundy background and the reference's coloured ink/paper character. Subtle ink registration offsets and print texture are welcome, but they must be controlled and must not shift the whole image off-canvas.

## Current reproducible symptoms reported by the user
1. Immediately after adding the effect, the result becomes black-and-white instead of coloured.
2. The image/subject shifts strongly toward the far left, partially or largely outside the frame.
3. The output looks like a second copy of the source is laid over the processed image (ghosting/double exposure).
4. Earlier fixes only changed parameter defaults/ranges and did not solve the problem. Treat the root cause as unknown until traced in code.

## Must-have behavior
- Default output is coloured; grayscale must not be forced on by default.
- Keep source image geometry and framing exactly stable. Registration offset should affect individual ink plates only, in a small range, and must not translate the whole output.
- No unintended duplicate-source overlay. If a `Blend With Original` control exists, its default and blend math must not cause ghosting. Verify that opacity/alpha is not applied twice.
- Preserve image details and silhouettes; avoid overexposure, white washout, crushed black, or flat monochrome.
- The effect must work on ordinary AE footage/layers and render correctly at different frame sizes, pixel aspect ratios, and supported bit depths.
- Preserve the existing project/build structure where possible; do not rename files, effect match names, PiPL identity, or the build workflow casually. The project is meant to build as one native `.aex` plugin.

## Required debugging steps (do these, not guesses)
1. Trace the full path from AE `SmartRender` input world to output world and into the core pixel processor.
2. Verify row stride, pixel format, channel order, alpha handling, bounds, width/height, and whether input/output worlds have different origins or dimensions. Confirm no incorrect use of `origin_x`, `origin_y`, `extent_hint`, rowbytes, or normalized coordinates causes a left shift.
3. Inspect every place the output is converted to grayscale, where ink colours are sampled or defaulted, and where `showSource`, blend mode, opacity, and `Blend With Original` are applied. Explain exactly why the observed black-and-white and ghosting could happen in this code.
4. Check whether AE parameter IDs/enums are stable and whether inserting/reordering parameters causes old saved values to be interpreted as different controls. Keep unique, stable IDs and ensure `NUM_PARAMS`, setup order, and render reads agree.
5. Verify registration offset is applied to individual ink masks/plates only; clamp sampling safely at image boundaries and never displace the whole canvas unintentionally.
6. Add a deterministic CPU/unit test or a small standalone harness for core processing if feasible: neutral settings should preserve the same frame bounds and colour; disabling ink offsets should produce zero displacement; setting original blend to 0 should not add the source a second time.
7. Run a clean Windows x64 MSVC/GitHub Actions build and inspect logs. Verify `.aex` architecture, PiPL/resource, exported entry point, and match name. Do not claim build success unless it actually succeeds.
8. If After Effects cannot be run in your environment, state that limitation plainly and provide a precise manual test plan; do not pretend the plugin was tested in AE.

## Deliverables
- A complete updated repository (not isolated code snippets) preserving its build workflow.
- A short `CHANGELOG.md` listing the root causes found, files changed, and why each fix addresses a symptom.
- Build logs or verification output if available, and clear separation between verified and unverified claims.
- Keep all reference images and settings in the handoff bundle for visual comparison.

## How to use the references
- Compare `references/01_before/IMG_4003.jpeg` with `references/02_after/IMG_4010.jpeg`.
- Inspect all three settings screenshots at native resolution; do not invent parameter values that are not visible.
- The after image is the visual target, not a reason to hardcode that exact image. The plugin must process arbitrary input images.
- Do not replace the real print algorithm with a simple grayscale filter, generic tint, or overlay stack.

Please inspect the actual source files before editing, identify root causes, implement and validate a proper fix, then return the complete repository as a ZIP.
