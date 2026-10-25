# Risograph reference pack for RisoPrint

This folder is an analysis/reference pack, not original C++ source code. `Risograph.aex.rzdb` is the user's original Rizin database artifact. The analysis files contain extracted strings, symbol names, and preliminary technical observations; claims should be verified against the database and screenshots.

## How to use
1. Read `GOOGLE_AI_STUDIO_PROMPT.md` from top to bottom.
2. Add the original plugin screenshots to `images/`.
3. Open this entire repository in Google AI Studio and ask it to inspect this reference pack and implement the supported behavior in the existing RisoPrint codebase.
4. Require it to preserve the current CMake / GitHub Actions structure and verify the resulting AEX rather than claiming success from a successful compile alone.

The `.rzdb` file is a database used for reverse-engineering, not a guarantee of recoverable original source code. The plugin implementation must be an independent reimplementation based on evidence.
