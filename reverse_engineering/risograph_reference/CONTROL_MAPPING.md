# RisoPrint — Complete Control Mapping & Verification Matrix

This matrix maps every observable parameter from the reference plugin `Risograph` to its evidentiary source, internal parameter ID, implementation location, and verification test status.

## Evidence Legend
- **CONFIRMED**: Backed by After Effects 2023 UI screenshots (`reverse_engineering/risograph_reference/images/`) and recovered code strings.
- **STRONGLY INDICATED**: Derived from recovered shader strings and function signatures.
- **INFERRED**: Logical mathematical extension consistent with observable behavior.

---

## Complete Control Inventory & Implementation Mapping

### 1. Root & Presets
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| Shuffle Seed | CONFIRMED | `ID_SHUFFLE_SEED` | 0 | Int Slider [0 .. 100000] | `resoprint::RisoApplySeed` | Verified |
| Press Preset | CONFIRMED | `ID_PRESS_PRESET` | 1 (Custom) | Popup (29 presets) | `resoprint::ApplyPressPreset` | Verified |

### 2. Group: Inspect
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| Plate Proof | CONFIRMED | `ID_PLATE_PROOF` | 1 (Off) | Popup (7 choices) | `resoprint::RisoApplyProofMode` | Verified |
| Magnify Zoom (x) | CONFIRMED | `ID_MAGNIFY_ZOOM` | 1.00 | Float [1.0 .. 10.0] | `resoprint::RisoApplyMagnify` | Verified |
| Magnify Center X (%) | CONFIRMED | `ID_MAGNIFY_CENTER_X` | 50.0 | Float [0.0 .. 100.0] | `resoprint::RisoApplyMagnify` | Verified |
| Magnify Center Y (%) | CONFIRMED | `ID_MAGNIFY_CENTER_Y` | 50.0 | Float [0.0 .. 100.0] | `resoprint::RisoApplyMagnify` | Verified |
| Cyan Plate | CONFIRMED | `ID_CYAN_PLATE_EN` | true | Checkbox | `resoprint::RisoApplyPlateMask` | Verified |
| Magenta Plate | CONFIRMED | `ID_MAGENTA_PLATE_EN`| true | Checkbox | `resoprint::RisoApplyPlateMask` | Verified |
| Yellow Plate | CONFIRMED | `ID_YELLOW_PLATE_EN` | true | Checkbox | `resoprint::RisoApplyPlateMask` | Verified |
| Black Plate | CONFIRMED | `ID_BLACK_PLATE_EN`  | true | Checkbox | `resoprint::RisoApplyPlateMask` | Verified |
| Halo Half-Res Speed | CONFIRMED | `ID_HALO_HALF_RES`   | false | Checkbox | `resoprint::RisoApplyHalo` | Verified |

### 3. Group: Machine & Paper
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| Machine Profile | CONFIRMED | `ID_MACHINE_PROFILE` | 1 (Custom) | Popup (12 models) | `resoprint::ApplyMachineProfile` | Verified |
| Print Resolution | CONFIRMED | `ID_PRINT_RESOLUTION`| 1 (Custom) | Popup (5 choices) | `resoprint::ResolveScreenDpi` | Verified |
| Paper Type | CONFIRMED | `ID_PAPER_TYPE` | 1 (Custom) | Popup (6 types) | `resoprint::RisoSamplePaperTexture` | Verified |
| Background | CONFIRMED | `ID_BACKGROUND_MODE` | 1 (Paper) | Popup (Paper, Transparent, Original) | `resoprint::RisoCompositeBackground` | Verified |
| Spot Ink Kit | CONFIRMED | `ID_SPOT_INK_KIT` | 1 (Custom) | Popup (9 kits) | `resoprint::ApplySpotKit` | Verified |
| Working Space | CONFIRMED | `ID_WORKING_SPACE` | 1 (Passthrough) | Popup (Passthrough, Linearize sRGB) | `resoprint::RisoWorkingColorSpace` | Verified |

### 4. Group: Color Correction
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| Brightness | CONFIRMED | `ID_BRIGHTNESS` | 0.0 | Float [-100.0 .. +100.0] | `resoprint::RisoAdjustColor` | Verified |
| Contrast | CONFIRMED | `ID_CONTRAST` | 0.0 | Float [-100.0 .. +100.0] | `resoprint::RisoAdjustColor` | Verified |
| Hue | CONFIRMED | `ID_HUE` | 0.0 | Float [-180.0 .. +180.0] | `resoprint::RisoAdjustColor` | Verified |
| Saturation | CONFIRMED | `ID_SATURATION` | 0.0 | Float [-100.0 .. +100.0] | `resoprint::RisoAdjustColor` | Verified |
| HSV Value | CONFIRMED | `ID_HSV_VALUE` | 0.0 | Float [-100.0 .. +100.0] | `resoprint::RisoAdjustColor` | Verified |
| Gamma | CONFIRMED | `ID_INPUT_GAMMA` | 1.00 | Float [0.10 .. 3.00] | `resoprint::RisoAdjustColor` | Verified |

### 5. Group: Ink Palette
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| Color Mode | CONFIRMED | `ID_COLOR_MODE` | 1 (CMYK) | Popup (CMYK, Spot Riso) | `resoprint::RisoDecompose` | Verified |
| Spot Separation | CONFIRMED | `ID_SPOT_SEPARATION`| 1 (Greedy) | Popup (Greedy, Balanced Mix) | `resoprint::RisoSpotDecompose` | Verified |
| Line Art Plate | CONFIRMED | `ID_LINE_ART_PLATE` | 1 (Off) | Popup (Off, Black, Plate 1..4) | `resoprint::RisoApplyLineArt` | Verified |
| Line Art Threshold | CONFIRMED | `ID_LINE_ART_THRESH`| 55.0 | Float [0.0 .. 100.0] | `resoprint::RisoLineArtMask` | Verified |
| Line Art Strength | CONFIRMED | `ID_LINE_ART_STRENGTH`| 100.0 | Float [0.0 .. 100.0] | `resoprint::RisoLineArtMask` | Verified |
| Active Plates (2-4)| CONFIRMED | `ID_ACTIVE_PLATES` | 4 | Int Slider [2 .. 4] | `resoprint::RisoSpotDecompose` | Verified |
| Coverage | CONFIRMED | `ID_COVERAGE` | 100.0 | Float [0.0 .. 200.0] | `resoprint::RisoApplyCoverage` | Verified |
| Cyan Ink | CONFIRMED | `ID_INK_CYAN` | #00A0E9 | Color | `resoprint::RisoInkColor` | Verified |
| Magenta Ink | CONFIRMED | `ID_INK_MAGENTA` | #E4007F | Color | `resoprint::RisoInkColor` | Verified |
| Yellow Ink | CONFIRMED | `ID_INK_YELLOW` | #FFF100 | Color | `resoprint::RisoInkColor` | Verified |
| Black Ink | CONFIRMED | `ID_INK_BLACK` | #231815 | Color | `resoprint::RisoInkColor` | Verified |
| Plate 1..4 Ink | CONFIRMED | `ID_SPOT_INK_1..4` | 1 (Custom) | Popup (40+ Spot inks) | `resoprint::ResolveSpotInk` | Verified |
| Black Boost | CONFIRMED | `ID_BLACK_BOOST` | 100.0 | Float [0.0 .. 200.0] | `resoprint::RisoApplyBlackBoost` | Verified |
| Riso Tint | CONFIRMED | `ID_RISO_TINT` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoApplyTint` | Verified |
| Overprint Mode | CONFIRMED | `ID_OVERPRINT_MODE` | 1 (Multiply)| Popup (Multiply, Beer-Lambert) | `resoprint::RisoCompositeInks` | Verified |
| Cyan..Black Translucency | CONFIRMED | `ID_TRANSLUCENCY_C..K`| 85.0 | Float [0.0 .. 100.0] | `resoprint::RisoBeerLambert` | Verified |
| Total Coverage Limit (%) | CONFIRMED | `ID_TAC_LIMIT` | 300.0 | Float [100.0 .. 400.0] | `resoprint::RisoApplyTACLimit` | Verified |
| Ink Pooling | CONFIRMED | `ID_INK_POOLING` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoApplyPooling` | Verified |
| Halo Radius (px) | CONFIRMED | `ID_HALO_RADIUS` | 0.0 | Float [0.0 .. 50.0] | `resoprint::RisoComputeHalo` | Verified |
| Halo Strength | CONFIRMED | `ID_HALO_STRENGTH` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoApplyHalo` | Verified |
| Cyan..Black Curve | CONFIRMED | `ID_INK_CURVE_C..K`| 1.00 | Float [0.10 .. 3.00] | `resoprint::RisoApplyPlateCurve` | Verified |

### 6. Group: Halftone
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| Screen Type | CONFIRMED | `ID_SCREEN_TYPE` | 1 (Voronoi) | Popup (7 screen types) | `resoprint::RisoHalftoneScreen` | Verified |
| Screen Density | CONFIRMED | `ID_SCREEN_DENSITY` | 200.0 | Float [10.0 .. 600.0] | `resoprint::RisoHalftoneScreen` | Verified |
| Dot Shape | CONFIRMED | `ID_DOT_SHAPE` | 1 (Circle) | Popup (Circle, Ellipse, Diamond, Square) | `resoprint::RisoCellDist` | Verified |
| Dot Hardness | CONFIRMED | `ID_DOT_HARDNESS` | 110.0 | Float [0.0 .. 200.0] | `resoprint::RisoApplyThreshold` | Verified |
| Softness | CONFIRMED | `ID_SOFTNESS` | 23.0 | Float [0.0 .. 100.0] | `resoprint::RisoApplyThreshold` | Verified |
| Distortion | CONFIRMED | `ID_DISTORTION` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoGridDistortion` | Verified |
| Dot Stretch | CONFIRMED | `ID_DOT_STRETCH` | 0.0 | Float [-100.0 .. +100.0]| `resoprint::RisoApplyAspect` | Verified |
| Edge Softness | CONFIRMED | `ID_EDGE_SOFTNESS` | 30.0 | Float [0.0 .. 100.0] | `resoprint::RisoApplyEdgeSoftness` | Verified |
| Dot Gain | CONFIRMED | `ID_DOT_GAIN` | 1.00 | Float [0.50 .. 3.00] | `resoprint::RisoApplyDotGain` | Verified |
| Cyan..Black Angle | CONFIRMED | `ID_ANGLE_C..K` | 15°, 75°, 0°, 45° | Angle Slider | `resoprint::RisoComputeGridPos` | Verified |
| Cyan..Black Weight | CONFIRMED | `ID_WEIGHT_C..K` | 128, 109, 100, 112 | Float [0.0 .. 200.0] | `resoprint::RisoChannelWeight` | Verified |
| Drum Animation Speed | CONFIRMED | `ID_DRUM_ANIM_SPEED` | 0.0 | Float [0.0 .. 10.0] | `resoprint::RisoDrumPhase` | Verified |
| Rich Blacks | CONFIRMED | `ID_RICH_BLACKS` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoRichBlacks` | Verified |

### 7. Group: Plate Alignment
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| All Plates Offset X/Y | CONFIRMED | `ID_ALL_OFFSET_X/Y` | 0.0, 0.0 | Float [-100.0 .. +100.0] | `resoprint::RisoApplyPlateXform` | Verified |
| All Plates Rotation | CONFIRMED | `ID_ALL_ROTATION` | 0.00 | Angle / Float [-180° .. 180°] | `resoprint::RisoApplyPlateXform` | Verified |
| All Plates Skew | CONFIRMED | `ID_ALL_SKEW` | 0.000 | Float [-1.0 .. +1.0] | `resoprint::RisoApplyPlateXform` | Verified |
| Per-Plate Offset X/Y | CONFIRMED | `ID_OFFSET_C/M/Y/K_X/Y` | 0.0, 0.0 | Float [-50.0 .. +50.0] | `resoprint::RisoComputeGridPos` | Verified |
| Per-Plate Rotation | CONFIRMED | `ID_ROT_C/M/Y/K` | 0.00 | Angle / Float [-45° .. 45°] | `resoprint::RisoApplyPlateXform` | Verified |
| Per-Plate Skew | CONFIRMED | `ID_SKEW_C/M/Y/K` | 0.000 | Float [-0.5 .. +0.5] | `resoprint::RisoApplyPlateXform` | Verified |

### 8. Group: Paper & Grain
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| Paper Color | CONFIRMED | `ID_PAPER_COLOR` | #FAF7F0 | Color | `resoprint::RisoPaperBase` | Verified |
| Paper Grain | CONFIRMED | `ID_PAPER_GRAIN` | 20.0 | Float [0.0 .. 100.0] | `resoprint::RisoPaperStructure` | Verified |
| Grain Scale | CONFIRMED | `ID_GRAIN_SCALE` | 50.0 | Float [1.0 .. 200.0] | `resoprint::RisoPaperStructure` | Verified |
| Sheet Warp | CONFIRMED | `ID_SHEET_WARP` | 15.0 | Float [0.0 .. 100.0] | `resoprint::RisoCrumpleLookup` | Verified |
| Aging Amount | CONFIRMED | `ID_AGING_AMOUNT` | 15.0 | Float [0.0 .. 100.0] | `resoprint::RisoPaperAging` | Verified |
| Aging Color | CONFIRMED | `ID_AGING_COLOR` | #8A5A2B | Color | `resoprint::RisoPaperAging` | Verified |

### 9. Group: Print Wear
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| Sun Bleach | CONFIRMED | `ID_SUN_BLEACH` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoSunBleach` | Verified |
| Bleach Direction | CONFIRMED | `ID_BLEACH_DIRECTION` | 0.0° | Angle Slider | `resoprint::RisoSunBleach` | Verified |
| Ink Variance | CONFIRMED | `ID_INK_VARIANCE` | 15.0 | Float [0.0 .. 100.0] | `resoprint::RisoInkInconsistency` | Verified |
| Ink Flecks | CONFIRMED | `ID_INK_FLECKS` | 10.0 | Float [0.0 .. 100.0] | `resoprint::RisoComputeSpecks` | Verified |
| Fleck Scale | CONFIRMED | `ID_FLECK_SCALE` | 50.0 | Float [1.0 .. 200.0] | `resoprint::RisoComputeSpecks` | Verified |
| Ink Blots | CONFIRMED | `ID_INK_BLOTS` | 5.0 | Float [0.0 .. 100.0] | `resoprint::RisoInkBlots` | Verified |
| Roller Streaks | CONFIRMED | `ID_ROLLER_STREAKS` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoRollerModulation` | Verified |
| Streak Frequency | CONFIRMED | `ID_STREAK_FREQ` | 12.0 | Float [1.0 .. 50.0] | `resoprint::RisoRollerModulation` | Verified |
| Streak Angle | CONFIRMED | `ID_STREAK_ANGLE` | 90.0° | Angle Slider | `resoprint::RisoRollerModulation` | Verified |
| Ghost Offset X/Y | CONFIRMED | `ID_GHOST_OFFSET_X/Y`| 0.0, 0.0 | Float [-50.0 .. +50.0] | `resoprint::RisoGhostOverlay` | Verified |
| Ghost Opacity | CONFIRMED | `ID_GHOST_OPACITY` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoGhostOverlay` | Verified |
| Ink Reticulation | CONFIRMED | `ID_INK_RETICULATION`| 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoReticulation` | Verified |
| Reticulation Scale | CONFIRMED | `ID_RETIC_SCALE` | 60.0 | Float [10.0 .. 200.0] | `resoprint::RisoReticulation` | Verified |
| Impressions | CONFIRMED | `ID_IMPRESSIONS` | 1 | Int Slider [1 .. 10] | `resoprint::RisoImpressions` | Verified |
| Impression Jitter (%) | CONFIRMED | `ID_IMPRESSION_JITTER` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoImpressions` | Verified |
| Ink Drift (%) | CONFIRMED | `ID_INK_DRIFT` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoInkDrift` | Verified |
| Reverse Print | CONFIRMED | `ID_REVERSE_PRINT` | false | Checkbox | `resoprint::RisoReversePrint` | Verified |

### 10. Group: Lighting & Finish
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| Light Intensity | CONFIRMED | `ID_LIGHT_INTENSITY` | 0.0 | Float [0.0 .. 100.0] | `resoprint::RisoArtificialLamp` | Verified |
| Light Angle | CONFIRMED | `ID_LIGHT_ANGLE` | 45.0° | Angle Slider | `resoprint::RisoArtificialLamp` | Verified |
| Light Elevation | CONFIRMED | `ID_LIGHT_ELEVATION` | 45.0 | Float [0.0 .. 90.0] | `resoprint::RisoArtificialLamp` | Verified |
| Ambient Light | CONFIRMED | `ID_AMBIENT_LIGHT` | 50.0 | Float [0.0 .. 100.0] | `resoprint::RisoArtificialLamp` | Verified |
| 3-Point Lighting | CONFIRMED | `ID_THREE_POINT_LIGHT`| 30.0 | Float [0.0 .. 100.0] | `resoprint::RisoStudioLighting` | Verified |

### 11. Group: Output
| Reference Label | Evidence | RisoPrint Parameter ID | Default | Type / Range | Implementation Function | Test Status |
|---|---|---|---|---|---|---|
| Output Gamma | CONFIRMED | `ID_OUTPUT_GAMMA` | 1.30 | Float [0.50 .. 2.50] | `resoprint::RisoOutputGamma` | Verified |
| Output Quality | CONFIRMED | `ID_OUTPUT_QUALITY` | 3 (High) | Popup (Draft, Standard, High, Ultra) | `resoprint::RisoQualityOctaves` | Verified |
| Press Fidelity | CONFIRMED | `ID_PRESS_FIDELITY` | 10.0 | Float [0.0 .. 100.0] | `resoprint::RisoPressFidelity` | Verified |
| Soft-Proof | CONFIRMED | `ID_SOFT_PROOF` | 1 (Off) | Popup (4 choices) | `resoprint::RisoSoftProofProfile`| Verified |
| Negative Output | CONFIRMED | `ID_NEGATIVE_OUTPUT` | false | Checkbox | `resoprint::RisoNegativeOutput` | Verified |
