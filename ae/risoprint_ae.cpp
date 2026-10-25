#include "AEConfig.h"
#include "AE_Effect.h"
#include "AE_EffectSuites.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "entry.h"
#include "risoprint_core.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define BUG_VERSION 0
#define STAGE_VERSION PF_Stage_RELEASE
#define BUILD_VERSION 1

// ---------------------------------------------------------------------------
// Parameter Enum: Strict 1:1 Correspondence with ParamsSetup
// ---------------------------------------------------------------------------
enum {
    ID_SHUFFLE_SEED = 1,
    ID_PRESS_PRESET,

    // Inspect Group
    ID_TOPIC_INSPECT_START,
    ID_PLATE_PROOF,
    ID_MAGNIFY_ZOOM,
    ID_MAGNIFY_CENTER_X,
    ID_MAGNIFY_CENTER_Y,
    ID_CYAN_PLATE_EN,
    ID_MAGENTA_PLATE_EN,
    ID_YELLOW_PLATE_EN,
    ID_BLACK_PLATE_EN,
    ID_HALO_HALF_RES,
    ID_TOPIC_INSPECT_END,

    // Machine & Paper Group
    ID_TOPIC_MACHINE_START,
    ID_MACHINE_PROFILE,
    ID_PRINT_RESOLUTION,
    ID_PAPER_TYPE,
    ID_BACKGROUND_MODE,
    ID_SPOT_INK_KIT,
    ID_WORKING_SPACE,
    ID_TOPIC_MACHINE_END,

    // Color Correction Group
    ID_TOPIC_COLOR_START,
    ID_BRIGHTNESS,
    ID_CONTRAST,
    ID_HUE,
    ID_SATURATION,
    ID_HSV_VALUE,
    ID_GAMMA,
    ID_TOPIC_COLOR_END,

    // Ink Palette Group
    ID_TOPIC_INK_START,
    ID_COLOR_MODE,
    ID_SPOT_SEPARATION,
    ID_LINE_ART_PLATE,
    ID_LINE_ART_THRESH,
    ID_LINE_ART_STRENGTH,
    ID_ACTIVE_PLATES,
    ID_COVERAGE,
    ID_INK_CYAN,
    ID_INK_MAGENTA,
    ID_INK_YELLOW,
    ID_INK_BLACK,
    ID_PLATE_1_INK,
    ID_PLATE_2_INK,
    ID_PLATE_3_INK,
    ID_PLATE_4_INK,
    ID_BLACK_BOOST,
    ID_RISO_TINT,
    ID_OVERPRINT_MODE,
    ID_CYAN_TRANSLUCENCY,
    ID_MAGENTA_TRANSLUCENCY,
    ID_YELLOW_TRANSLUCENCY,
    ID_BLACK_TRANSLUCENCY,
    ID_TAC_LIMIT,
    ID_INK_POOLING,
    ID_HALO_RADIUS,
    ID_HALO_STRENGTH,
    ID_CYAN_CURVE,
    ID_MAGENTA_CURVE,
    ID_YELLOW_CURVE,
    ID_BLACK_CURVE,
    ID_TOPIC_INK_END,

    // Halftone Group
    ID_TOPIC_HALFTONE_START,
    ID_SCREEN_TYPE,
    ID_SCREEN_DENSITY,
    ID_DOT_SHAPE,
    ID_DOT_HARDNESS,
    ID_SOFTNESS,
    ID_DISTORTION,
    ID_DOT_STRETCH,
    ID_EDGE_SOFTNESS,
    ID_DOT_GAIN,
    ID_CYAN_ANGLE,
    ID_MAGENTA_ANGLE,
    ID_YELLOW_ANGLE,
    ID_BLACK_ANGLE,
    ID_CYAN_WEIGHT,
    ID_MAGENTA_WEIGHT,
    ID_YELLOW_WEIGHT,
    ID_BLACK_WEIGHT,
    ID_DRUM_ANIM_SPEED,
    ID_RICH_BLACKS,
    ID_TOPIC_HALFTONE_END,

    // Plate Alignment Group
    ID_TOPIC_ALIGN_START,
    ID_ALL_OFFSET_X,
    ID_ALL_OFFSET_Y,
    ID_ALL_ROTATION,
    ID_ALL_SKEW,
    ID_CYAN_OFFSET_X,
    ID_CYAN_OFFSET_Y,
    ID_MAGENTA_OFFSET_X,
    ID_MAGENTA_OFFSET_Y,
    ID_YELLOW_OFFSET_X,
    ID_YELLOW_OFFSET_Y,
    ID_BLACK_OFFSET_X,
    ID_BLACK_OFFSET_Y,
    ID_CYAN_ROTATION,
    ID_MAGENTA_ROTATION,
    ID_YELLOW_ROTATION,
    ID_BLACK_ROTATION,
    ID_CYAN_SKEW,
    ID_MAGENTA_SKEW,
    ID_YELLOW_SKEW,
    ID_BLACK_SKEW,
    ID_TOPIC_ALIGN_END,

    // Paper & Grain Group
    ID_TOPIC_PAPER_START,
    ID_PAPER_COLOR,
    ID_PAPER_GRAIN,
    ID_GRAIN_SCALE,
    ID_SHEET_WARP,
    ID_AGING_AMOUNT,
    ID_AGING_COLOR,
    ID_TOPIC_PAPER_END,

    // Print Wear Group
    ID_TOPIC_WEAR_START,
    ID_SUN_BLEACH,
    ID_BLEACH_DIRECTION,
    ID_INK_VARIANCE,
    ID_INK_FLECKS,
    ID_FLECK_SCALE,
    ID_INK_BLOTS,
    ID_ROLLER_STREAKS,
    ID_STREAK_FREQ,
    ID_STREAK_ANGLE,
    ID_GHOST_OFFSET_X,
    ID_GHOST_OFFSET_Y,
    ID_GHOST_OPACITY,
    ID_INK_RETICULATION,
    ID_RETIC_SCALE,
    ID_IMPRESSIONS,
    ID_IMPRESSION_JITTER,
    ID_INK_DRIFT,
    ID_REVERSE_PRINT,
    ID_TOPIC_WEAR_END,

    // Lighting Group
    ID_TOPIC_LIGHT_START,
    ID_LIGHT_INTENSITY,
    ID_LIGHT_ANGLE,
    ID_LIGHT_ELEVATION,
    ID_AMBIENT_LIGHT,
    ID_THREE_POINT_LIGHT,
    ID_TOPIC_LIGHT_END,

    // Output Group
    ID_TOPIC_OUTPUT_START,
    ID_OUTPUT_GAMMA,
    ID_OUTPUT_QUALITY,
    ID_PRESS_FIDELITY,
    ID_SOFT_PROOF,
    ID_NEGATIVE_OUTPUT,
    ID_BLEND_ORIGINAL,
    ID_TOPIC_OUTPUT_END,

    NUM_PARAMS = ID_TOPIC_OUTPUT_END
};

// ---------------------------------------------------------------------------
// UI Popup Choices (Strictly Matching After Effects Screenshots)
// ---------------------------------------------------------------------------

static const char* PRESS_PRESETS_STR =
    "Custom|Soft Studio|1970s Zine|Blown-out Punk|Newsprint|RisoFest 2-color|"
    "Letterpress Indigo|Bright Comic|Vintage Magazine|Midnight Mimeograph|"
    "Fluoro Shop Window|Library Card|Acid House Flyer|Botanical Field Guide|"
    "Campus Bulletin|Neon Noir|Kraft Market Bag|Seafoam Catalog|Kids Workshop|"
    "Blueprint Proof|Sun-Baked Poster|Copyshop Club Night|Cotton Art Print|"
    "Forest Field Notes|Candy Duotone|Archive Microfiche|Weathered Map|"
    "Gallery Risotype|Torn Edge Flyer";

static const char* PLATE_PROOF_STR =
    "Off (Composite)|Dot Mask Only|Cyan Separation|Magenta Separation|"
    "Yellow Separation|Black Separation|4-Up Proof Grid";

static const char* MACHINE_PROFILES_STR =
    "Custom|GR3750 (1990s, 400 DPI)|EZ591 (600e, single-drum)|MZ-790 (600e, 2-color)|"
    "RZ970 (600x600, one-drum)|SF5350 (300x600, modern)|CR1610 (Compact, 300 DPI)|"
    "GR3770 (Legacy, 600 DPI)|RP3700 (A3, 600 DPI)|CV3230 (Value, 300x600 DPI)|"
    "SF9450 (Flagship, 600 DPI)|MH9450 (Modern, 600 DPI, 2-color)";

static const char* PRINT_RES_STR =
    "Custom (use Scale)|150 DPI (Newspaper)|300 DPI (Magazine)|"
    "600 DPI (High-quality)|1200 DPI (Fine art)";

static const char* PAPER_TYPES_STR =
    "Custom|Newsprint|Cotton Rag|Kraft|Papyrus|Smooth Coated";

static const char* BACKGROUND_STR =
    "Paper|Transparent|Original Image";

static const char* SPOT_INK_KITS_STR =
    "Custom|Risograph Pink+Blue|Cool Trio (Mint+Sky+Teal)|Earthy (Brown+Burgundy+Gold)|"
    "Twilight (Indigo+Plum+Crimson)|Sunset (Sunflower+Pumpkin+Red)|"
    "Forest (Hunter+Sea+Brown)|Mint+Pink Duo|Cyan+Magenta Duo";

static const char* WORKING_SPACE_STR =
    "Passthrough|Linearize sRGB";

static const char* COLOR_MODE_STR =
    "CMYK|Spot Riso";

static const char* SPOT_SEP_STR =
    "Greedy (Legacy)|Balanced Mix";

static const char* LINE_ART_PLATE_STR =
    "Off|Black/K|Plate 1|Plate 2|Plate 3|Plate 4";

static const char* SPOT_INKS_STR =
    "Custom|Black|Blue|Bright Red|Brown|Burgundy|Crimson|Federal Blue|Flat Gold|"
    "Fluorescent Green|Fluorescent Orange|Fluorescent Pink|Fluorescent Red|"
    "Fluorescent Yellow|Forest|Granite|Green|Hunter Green|Indigo|Ivy|Lake|"
    "Light Gray|Light Lime|Light Mauve|Light Teal|Marine Red|Medium Blue|"
    "Melon|Metallic Gold|Midnight|Mint|Orange|Orchid|Paprika|Pine|Plum|"
    "Pumpkin|Purple|Raisin|Raspberry|Red|Scarlet|Sea Blue|Seafoam|Sky Blue|"
    "Slate|Smoky Teal|Spruce|Steel|Sunflower|Sunshine|Teal|Turquoise|Violet|"
    "Wine|Yellow";

static const char* OVERPRINT_STR =
    "Multiply|Beer-Lambert";

static const char* SCREEN_TYPES_STR =
    "Voronoi|Bayer 8x8|Blue-noise|Line Screen|Stippled|Atkinson (sparse)|Floyd-Steinberg (clustered)";

static const char* DOT_SHAPES_STR =
    "Circle|Ellipse|Diamond|Square";

static const char* OUTPUT_QUALITY_STR =
    "Draft|Standard|High|Ultra";

static const char* SOFT_PROOF_STR =
    "Off|Standard News|Uncoated Book|Heavy Cardstock";

// ---------------------------------------------------------------------------
// ParamsSetup: Exact 1:1 Sequential Registration
// ---------------------------------------------------------------------------

static PF_Err ParamsSetup(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output
) {
    PF_Err err = PF_Err_NONE;
    PF_ParamDef def;

    auto slider = [&](const char* n, double vmin, double vmax, double smin, double smax, double d, short prec, int id) -> PF_Err {
        AEFX_CLR_STRUCT(def);
        def.param_type = PF_Param_FLOAT_SLIDER;
        PF_STRCPY(def.name, n);
        def.u.fs_d.valid_min = vmin;
        def.u.fs_d.valid_max = vmax;
        def.u.fs_d.slider_min = smin;
        def.u.fs_d.slider_max = smax;
        def.u.fs_d.value = d;
        def.u.fs_d.dephault = (PF_FpShort)d;
        def.u.fs_d.precision = prec;
        def.uu.id = id;
        return PF_ADD_PARAM(in_data, -1, &def);
    };

    auto int_slider = [&](const char* n, int vmin, int vmax, int smin, int smax, int d, int id) -> PF_Err {
        AEFX_CLR_STRUCT(def);
        def.param_type = PF_Param_SLIDER;
        PF_STRCPY(def.name, n);
        def.u.sd.valid_min = vmin;
        def.u.sd.valid_max = vmax;
        def.u.sd.slider_min = smin;
        def.u.sd.slider_max = smax;
        def.u.sd.value = d;
        def.u.sd.dephault = d;
        def.uu.id = id;
        return PF_ADD_PARAM(in_data, -1, &def);
    };

    auto check = [&](const char* n, bool d, int id) -> PF_Err {
        AEFX_CLR_STRUCT(def);
        def.param_type = PF_Param_CHECKBOX;
        PF_STRCPY(def.name, n);
        def.u.bd.value = d;
        def.u.bd.dephault = d;
        def.u.bd.u.nameptr = n;
        def.uu.id = id;
        return PF_ADD_PARAM(in_data, -1, &def);
    };

    auto popup = [&](const char* n, int count, int d, const char* choices, int id) -> PF_Err {
        AEFX_CLR_STRUCT(def);
        def.param_type = PF_Param_POPUP;
        PF_STRCPY(def.name, n);
        def.u.pd.num_choices = count;
        def.u.pd.value = d;
        def.u.pd.dephault = d;
        def.u.pd.u.namesptr = choices;
        def.uu.id = id;
        return PF_ADD_PARAM(in_data, -1, &def);
    };

    auto color = [&](const char* n, int r, int g, int b, int id) -> PF_Err {
        AEFX_CLR_STRUCT(def);
        def.param_type = PF_Param_COLOR;
        PF_STRCPY(def.name, n);
        def.u.cd.value.red = r;
        def.u.cd.value.green = g;
        def.u.cd.value.blue = b;
        def.u.cd.value.alpha = 255;
        def.u.cd.dephault = def.u.cd.value;
        def.uu.id = id;
        return PF_ADD_PARAM(in_data, -1, &def);
    };

    auto angle = [&](const char* n, double d, int id) -> PF_Err {
        AEFX_CLR_STRUCT(def);
        def.param_type = PF_Param_ANGLE;
        PF_STRCPY(def.name, n);
        def.u.ad.value = def.u.ad.dephault = (PF_Fixed)(d * 65536.0);
        def.uu.id = id;
        return PF_ADD_PARAM(in_data, -1, &def);
    };

    auto group_start = [&](const char* n, int id) -> PF_Err {
        AEFX_CLR_STRUCT(def);
        def.param_type = PF_Param_GROUP_START;
        PF_STRCPY(def.name, n);
        def.uu.id = id;
        return PF_ADD_PARAM(in_data, -1, &def);
    };

    auto group_end = [&](int id) -> PF_Err {
        AEFX_CLR_STRUCT(def);
        def.param_type = PF_Param_GROUP_END;
        def.uu.id = id;
        return PF_ADD_PARAM(in_data, -1, &def);
    };

    #define ADD(call) do { err = (call); if (err) return err; } while (0)

    // Top Level
    ADD(int_slider("Shuffle Seed", 0, 100000, 0, 1000, 0, ID_SHUFFLE_SEED));
    ADD(popup("Press Preset", 29, 1, PRESS_PRESETS_STR, ID_PRESS_PRESET));

    // Group: Inspect
    ADD(group_start("Inspect", ID_TOPIC_INSPECT_START));
    ADD(popup("Plate Proof", 7, 1, PLATE_PROOF_STR, ID_PLATE_PROOF));
    ADD(slider("Magnify Zoom (x)", 1.0, 10.0, 1.0, 5.0, 1.0, 2, ID_MAGNIFY_ZOOM));
    ADD(slider("Magnify Center X (%)", 0.0, 100.0, 0.0, 100.0, 50.0, 1, ID_MAGNIFY_CENTER_X));
    ADD(slider("Magnify Center Y (%)", 0.0, 100.0, 0.0, 100.0, 50.0, 1, ID_MAGNIFY_CENTER_Y));
    ADD(check("Cyan Plate", true, ID_CYAN_PLATE_EN));
    ADD(check("Magenta Plate", true, ID_MAGENTA_PLATE_EN));
    ADD(check("Yellow Plate", true, ID_YELLOW_PLATE_EN));
    ADD(check("Black Plate", true, ID_BLACK_PLATE_EN));
    ADD(check("Halo Half-Res Speed", false, ID_HALO_HALF_RES));
    ADD(group_end(ID_TOPIC_INSPECT_END));

    // Group: Machine & Paper
    ADD(group_start("Machine & Paper", ID_TOPIC_MACHINE_START));
    ADD(popup("Machine Profile", 12, 1, MACHINE_PROFILES_STR, ID_MACHINE_PROFILE));
    ADD(popup("Print Resolution", 5, 1, PRINT_RES_STR, ID_PRINT_RESOLUTION));
    ADD(popup("Paper Type", 6, 1, PAPER_TYPES_STR, ID_PAPER_TYPE));
    ADD(popup("Background", 3, 1, BACKGROUND_STR, ID_BACKGROUND_MODE));
    ADD(popup("Spot Ink Kit", 9, 1, SPOT_INK_KITS_STR, ID_SPOT_INK_KIT));
    ADD(popup("Working Space", 2, 1, WORKING_SPACE_STR, ID_WORKING_SPACE));
    ADD(group_end(ID_TOPIC_MACHINE_END));

    // Group: Color Correction
    ADD(group_start("Color Correction", ID_TOPIC_COLOR_START));
    ADD(slider("Brightness", -100.0, 100.0, -50.0, 50.0, 0.0, 1, ID_BRIGHTNESS));
    ADD(slider("Contrast", -100.0, 100.0, -50.0, 50.0, 0.0, 1, ID_CONTRAST));
    ADD(slider("Hue", -180.0, 180.0, -180.0, 180.0, 0.0, 1, ID_HUE));
    ADD(slider("Saturation", -100.0, 100.0, -50.0, 50.0, 0.0, 1, ID_SATURATION));
    ADD(slider("HSV Value", -100.0, 100.0, -50.0, 50.0, 0.0, 1, ID_HSV_VALUE));
    ADD(slider("Gamma", 0.10, 3.00, 0.50, 2.00, 1.00, 2, ID_GAMMA));
    ADD(group_end(ID_TOPIC_COLOR_END));

    // Group: Ink Palette
    ADD(group_start("Ink Palette", ID_TOPIC_INK_START));
    ADD(popup("Color Mode", 2, 1, COLOR_MODE_STR, ID_COLOR_MODE));
    ADD(popup("Spot Separation", 2, 1, SPOT_SEP_STR, ID_SPOT_SEPARATION));
    ADD(popup("Line Art Plate", 6, 1, LINE_ART_PLATE_STR, ID_LINE_ART_PLATE));
    ADD(slider("Line Art Threshold", 0.0, 100.0, 0.0, 100.0, 55.0, 1, ID_LINE_ART_THRESH));
    ADD(slider("Line Art Strength", 0.0, 100.0, 0.0, 100.0, 100.0, 1, ID_LINE_ART_STRENGTH));
    ADD(int_slider("Active Plates (2-4)", 2, 4, 2, 4, 4, ID_ACTIVE_PLATES));
    ADD(slider("Coverage", 0.0, 200.0, 0.0, 150.0, 100.0, 1, ID_COVERAGE));
    ADD(color("Cyan Ink", 0, 160, 233, ID_INK_CYAN));
    ADD(color("Magenta Ink", 228, 0, 127, ID_INK_MAGENTA));
    ADD(color("Yellow Ink", 255, 241, 0, ID_INK_YELLOW));
    ADD(color("Black Ink", 35, 24, 21, ID_INK_BLACK));
    ADD(popup("Plate 1 Ink", 56, 1, SPOT_INKS_STR, ID_PLATE_1_INK));
    ADD(popup("Plate 2 Ink", 56, 1, SPOT_INKS_STR, ID_PLATE_2_INK));
    ADD(popup("Plate 3 Ink", 56, 1, SPOT_INKS_STR, ID_PLATE_3_INK));
    ADD(popup("Plate 4 Ink", 56, 1, SPOT_INKS_STR, ID_PLATE_4_INK));
    ADD(slider("Black Boost", 0.0, 200.0, 0.0, 200.0, 100.0, 1, ID_BLACK_BOOST));
    ADD(slider("Riso Tint", 0.0, 100.0, 0.0, 100.0, 0.0, 1, ID_RISO_TINT));
    ADD(popup("Overprint Mode", 2, 1, OVERPRINT_STR, ID_OVERPRINT_MODE));
    ADD(slider("Cyan Translucency", 0.0, 100.0, 0.0, 100.0, 85.0, 1, ID_CYAN_TRANSLUCENCY));
    ADD(slider("Magenta Translucency", 0.0, 100.0, 0.0, 100.0, 85.0, 1, ID_MAGENTA_TRANSLUCENCY));
    ADD(slider("Yellow Translucency", 0.0, 100.0, 0.0, 100.0, 85.0, 1, ID_YELLOW_TRANSLUCENCY));
    ADD(slider("Black Translucency", 0.0, 100.0, 0.0, 100.0, 85.0, 1, ID_BLACK_TRANSLUCENCY));
    ADD(slider("Total Coverage Limit (%)", 100.0, 400.0, 150.0, 350.0, 300.0, 1, ID_TAC_LIMIT));
    ADD(slider("Ink Pooling", 0.0, 100.0, 0.0, 50.0, 0.0, 1, ID_INK_POOLING));
    ADD(slider("Halo Radius (px)", 0.0, 50.0, 0.0, 30.0, 0.0, 1, ID_HALO_RADIUS));
    ADD(slider("Halo Strength", 0.0, 100.0, 0.0, 100.0, 0.0, 1, ID_HALO_STRENGTH));
    ADD(slider("Cyan Curve", 0.10, 3.00, 0.50, 2.00, 1.00, 2, ID_CYAN_CURVE));
    ADD(slider("Magenta Curve", 0.10, 3.00, 0.50, 2.00, 1.00, 2, ID_MAGENTA_CURVE));
    ADD(slider("Yellow Curve", 0.10, 3.00, 0.50, 2.00, 1.00, 2, ID_YELLOW_CURVE));
    ADD(slider("Black Curve", 0.10, 3.00, 0.50, 2.00, 1.00, 2, ID_BLACK_CURVE));
    ADD(group_end(ID_TOPIC_INK_END));

    // Group: Halftone
    ADD(group_start("Halftone", ID_TOPIC_HALFTONE_START));
    ADD(popup("Screen Type", 7, 1, SCREEN_TYPES_STR, ID_SCREEN_TYPE));
    ADD(slider("Screen Density", 10.0, 600.0, 50.0, 400.0, 200.0, 1, ID_SCREEN_DENSITY));
    ADD(popup("Dot Shape", 4, 1, DOT_SHAPES_STR, ID_DOT_SHAPE));
    ADD(slider("Dot Hardness", 0.0, 200.0, 50.0, 150.0, 110.0, 1, ID_DOT_HARDNESS));
    ADD(slider("Softness", 0.0, 100.0, 0.0, 100.0, 23.0, 1, ID_SOFTNESS));
    ADD(slider("Distortion", 0.0, 100.0, 0.0, 50.0, 0.0, 1, ID_DISTORTION));
    ADD(slider("Dot Stretch", -100.0, 100.0, -50.0, 50.0, 0.0, 1, ID_DOT_STRETCH));
    ADD(slider("Edge Softness", 0.0, 100.0, 0.0, 100.0, 30.0, 1, ID_EDGE_SOFTNESS));
    ADD(slider("Dot Gain", 0.50, 3.00, 0.80, 2.00, 1.00, 2, ID_DOT_GAIN));
    ADD(angle("Cyan Angle", 15.0, ID_CYAN_ANGLE));
    ADD(angle("Magenta Angle", 75.0, ID_MAGENTA_ANGLE));
    ADD(angle("Yellow Angle", 0.0, ID_YELLOW_ANGLE));
    ADD(angle("Black Angle", 45.0, ID_BLACK_ANGLE));
    ADD(slider("Cyan Weight", 0.0, 200.0, 50.0, 150.0, 128.0, 1, ID_CYAN_WEIGHT));
    ADD(slider("Magenta Weight", 0.0, 200.0, 50.0, 150.0, 109.0, 1, ID_MAGENTA_WEIGHT));
    ADD(slider("Yellow Weight", 0.0, 200.0, 50.0, 150.0, 100.0, 1, ID_YELLOW_WEIGHT));
    ADD(slider("Black Weight", 0.0, 200.0, 50.0, 150.0, 112.0, 1, ID_BLACK_WEIGHT));
    ADD(slider("Drum Animation Speed", 0.0, 10.0, 0.0, 5.0, 0.0, 1, ID_DRUM_ANIM_SPEED));
    ADD(slider("Rich Blacks", 0.0, 100.0, 0.0, 100.0, 0.0, 1, ID_RICH_BLACKS));
    ADD(group_end(ID_TOPIC_HALFTONE_END));

    // Group: Plate Alignment
    ADD(group_start("Plate Alignment", ID_TOPIC_ALIGN_START));
    ADD(slider("All Plates Offset X (px)", -100.0, 100.0, -25.0, 25.0, 0.0, 1, ID_ALL_OFFSET_X));
    ADD(slider("All Plates Offset Y (px)", -100.0, 100.0, -25.0, 25.0, 0.0, 1, ID_ALL_OFFSET_Y));
    ADD(slider("All Plates Rotation", -180.0, 180.0, -45.0, 45.0, 0.0, 2, ID_ALL_ROTATION));
    ADD(slider("All Plates Skew", -1.0, 1.0, -0.2, 0.2, 0.0, 3, ID_ALL_SKEW));
    ADD(slider("Cyan Offset X", -50.0, 50.0, -10.0, 10.0, 0.0, 1, ID_CYAN_OFFSET_X));
    ADD(slider("Cyan Offset Y", -50.0, 50.0, -10.0, 10.0, 0.0, 1, ID_CYAN_OFFSET_Y));
    ADD(slider("Magenta Offset X", -50.0, 50.0, -10.0, 10.0, 0.0, 1, ID_MAGENTA_OFFSET_X));
    ADD(slider("Magenta Offset Y", -50.0, 50.0, -10.0, 10.0, 0.0, 1, ID_MAGENTA_OFFSET_Y));
    ADD(slider("Yellow Offset X", -50.0, 50.0, -10.0, 10.0, 0.0, 1, ID_YELLOW_OFFSET_X));
    ADD(slider("Yellow Offset Y", -50.0, 50.0, -10.0, 10.0, 0.0, 1, ID_YELLOW_OFFSET_Y));
    ADD(slider("Black Offset X", -50.0, 50.0, -10.0, 10.0, 0.0, 1, ID_BLACK_OFFSET_X));
    ADD(slider("Black Offset Y", -50.0, 50.0, -10.0, 10.0, 0.0, 1, ID_BLACK_OFFSET_Y));
    ADD(slider("Cyan Rotation", -45.0, 45.0, -15.0, 15.0, 0.0, 2, ID_CYAN_ROTATION));
    ADD(slider("Magenta Rotation", -45.0, 45.0, -15.0, 15.0, 0.0, 2, ID_MAGENTA_ROTATION));
    ADD(slider("Yellow Rotation", -45.0, 45.0, -15.0, 15.0, 0.0, 2, ID_YELLOW_ROTATION));
    ADD(slider("Black Rotation", -45.0, 45.0, -15.0, 15.0, 0.0, 2, ID_BLACK_ROTATION));
    ADD(slider("Cyan Skew", -0.5, 0.5, -0.1, 0.1, 0.0, 3, ID_CYAN_SKEW));
    ADD(slider("Magenta Skew", -0.5, 0.5, -0.1, 0.1, 0.0, 3, ID_MAGENTA_SKEW));
    ADD(slider("Yellow Skew", -0.5, 0.5, -0.1, 0.1, 0.0, 3, ID_YELLOW_SKEW));
    ADD(slider("Black Skew", -0.5, 0.5, -0.1, 0.1, 0.0, 3, ID_BLACK_SKEW));
    ADD(group_end(ID_TOPIC_ALIGN_END));

    // Group: Paper & Grain
    ADD(group_start("Paper & Grain", ID_TOPIC_PAPER_START));
    ADD(color("Paper Color", 250, 247, 240, ID_PAPER_COLOR));
    ADD(slider("Paper Grain", 0.0, 100.0, 0.0, 50.0, 20.0, 1, ID_PAPER_GRAIN));
    ADD(slider("Grain Scale", 1.0, 200.0, 20.0, 100.0, 50.0, 1, ID_GRAIN_SCALE));
    ADD(slider("Sheet Warp", 0.0, 100.0, 0.0, 50.0, 15.0, 1, ID_SHEET_WARP));
    ADD(slider("Aging Amount", 0.0, 100.0, 0.0, 50.0, 15.0, 1, ID_AGING_AMOUNT));
    ADD(color("Aging Color", 138, 90, 43, ID_AGING_COLOR));
    ADD(group_end(ID_TOPIC_PAPER_END));

    // Group: Print Wear
    ADD(group_start("Print Wear", ID_TOPIC_WEAR_START));
    ADD(slider("Sun Bleach", 0.0, 100.0, 0.0, 50.0, 0.0, 1, ID_SUN_BLEACH));
    ADD(angle("Bleach Direction", 0.0, ID_BLEACH_DIRECTION));
    ADD(slider("Ink Variance", 0.0, 100.0, 0.0, 50.0, 15.0, 1, ID_INK_VARIANCE));
    ADD(slider("Ink Flecks", 0.0, 100.0, 0.0, 50.0, 10.0, 1, ID_INK_FLECKS));
    ADD(slider("Fleck Scale", 1.0, 200.0, 20.0, 100.0, 50.0, 1, ID_FLECK_SCALE));
    ADD(slider("Ink Blots", 0.0, 100.0, 0.0, 30.0, 5.0, 1, ID_INK_BLOTS));
    ADD(slider("Roller Streaks", 0.0, 100.0, 0.0, 50.0, 0.0, 1, ID_ROLLER_STREAKS));
    ADD(slider("Streak Frequency (per page)", 1.0, 50.0, 4.0, 30.0, 12.0, 1, ID_STREAK_FREQ));
    ADD(angle("Streak Angle", 90.0, ID_STREAK_ANGLE));
    ADD(slider("Ghost Offset X (px)", -50.0, 50.0, -15.0, 15.0, 0.0, 1, ID_GHOST_OFFSET_X));
    ADD(slider("Ghost Offset Y (px)", -50.0, 50.0, -15.0, 15.0, 0.0, 1, ID_GHOST_OFFSET_Y));
    ADD(slider("Ghost Opacity", 0.0, 100.0, 0.0, 50.0, 0.0, 1, ID_GHOST_OPACITY));
    ADD(slider("Ink Reticulation", 0.0, 100.0, 0.0, 50.0, 0.0, 1, ID_INK_RETICULATION));
    ADD(slider("Reticulation Scale (cells)", 10.0, 200.0, 20.0, 100.0, 60.0, 1, ID_RETIC_SCALE));
    ADD(int_slider("Impressions", 1, 10, 1, 5, 1, ID_IMPRESSIONS));
    ADD(slider("Impression Jitter (%)", 0.0, 100.0, 0.0, 20.0, 0.0, 1, ID_IMPRESSION_JITTER));
    ADD(slider("Ink Drift (%)", 0.0, 100.0, 0.0, 20.0, 0.0, 1, ID_INK_DRIFT));
    ADD(check("Reverse Print (white on dark)", false, ID_REVERSE_PRINT));
    ADD(group_end(ID_TOPIC_WEAR_END));

    // Group: Lighting
    ADD(group_start("Lighting", ID_TOPIC_LIGHT_START));
    ADD(slider("Light Intensity", 0.0, 100.0, 0.0, 100.0, 0.0, 1, ID_LIGHT_INTENSITY));
    ADD(angle("Light Angle", 45.0, ID_LIGHT_ANGLE));
    ADD(slider("Light Elevation", 0.0, 90.0, 15.0, 75.0, 45.0, 1, ID_LIGHT_ELEVATION));
    ADD(slider("Ambient Light", 0.0, 100.0, 0.0, 100.0, 50.0, 1, ID_AMBIENT_LIGHT));
    ADD(slider("3-Point Lighting", 0.0, 100.0, 0.0, 100.0, 30.0, 1, ID_THREE_POINT_LIGHT));
    ADD(group_end(ID_TOPIC_LIGHT_END));

    // Group: Output
    ADD(group_start("Output", ID_TOPIC_OUTPUT_START));
    ADD(slider("Output Gamma", 0.50, 2.50, 0.80, 2.00, 1.30, 2, ID_OUTPUT_GAMMA));
    ADD(popup("Output Quality", 4, 3, OUTPUT_QUALITY_STR, ID_OUTPUT_QUALITY));
    ADD(slider("Press Fidelity", 0.0, 100.0, 0.0, 50.0, 10.0, 1, ID_PRESS_FIDELITY));
    ADD(popup("Soft-Proof", 4, 1, SOFT_PROOF_STR, ID_SOFT_PROOF));
    ADD(check("Negative Output", false, ID_NEGATIVE_OUTPUT));
    ADD(slider("Blend With Original (%)", 0.0, 100.0, 0.0, 100.0, 0.0, 1, ID_BLEND_ORIGINAL));
    ADD(group_end(ID_TOPIC_OUTPUT_END));

    #undef ADD

    out_data->num_params = NUM_PARAMS;
    return err;
}

// ---------------------------------------------------------------------------
// GlobalSetup
// ---------------------------------------------------------------------------

static PF_Err GlobalSetup(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output
) {
    PF_Err err = PF_Err_NONE;

    out_data->my_version = PF_VERSION(MAJOR_VERSION, MINOR_VERSION, BUG_VERSION, STAGE_VERSION, BUILD_VERSION);
    out_data->out_flags = PF_OutFlag_DEEP_COLOR_AWARE |
                          PF_OutFlag_WIDE_TIME_INPUT   |
                          PF_OutFlag_PIX_INDEPENDENT   |
                          PF_OutFlag_USE_OUTPUT_EXTENT;

    out_data->out_flags2 = PF_OutFlag2_SUPPORTS_SMART_RENDER |
                           PF_OutFlag2_FLOAT_COLOR_AWARE     |
                           PF_OutFlag2_DOESNT_NEED_EMPTY_PIXELS;

    return err;
}

// ---------------------------------------------------------------------------
// SmartRender & SmartPreRender
// ---------------------------------------------------------------------------

static PF_Err PreRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_PreRenderExtra* extra
) {
    PF_Err err = PF_Err_NONE;
    PF_RenderRequest req = extra->input->expected_request;
    req.preserve_rgb_of_zero_alpha = 1;

    PF_CheckoutResult in_result;
    err = extra->cb->checkout_layer(in_data->effect_ref,
                                    0, // input layer index
                                    0,
                                    &req,
                                    in_data->current_time,
                                    in_data->time_step,
                                    in_data->time_scale,
                                    &in_result);
    if (!err) {
        UnionLRect(&in_result.result_rect, &extra->output->result_rect);
        UnionLRect(&in_result.max_result_rect, &extra->output->max_result_rect);
    }
    return err;
}

static PF_Err SmartRender(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_SmartRenderExtra* extra
) {
    PF_Err err = PF_Err_NONE;
    PF_EffectWorld* srcW = nullptr;
    PF_EffectWorld* dstW = nullptr;

    ERR(extra->cb->checkout_layer_pixels(in_data->effect_ref, 0, &srcW));
    ERR(extra->cb->checkout_output(in_data->effect_ref, &dstW));

    if (err || !srcW || !dstW) return err;

    PF_PixelFormat format = PF_PixelFormat_ARGB32;
    PF_WorldSuite2* ws = nullptr;
    if (in_data->pica_basicP) {
        in_data->pica_basicP->AcquireSuite(kPFWorldSuite, kPFWorldSuiteVersion2, (const void**)&ws);
        if (ws) {
            ws->PF_GetPixelFormat(srcW, &format);
        }
    }

    int width = std::min(srcW->width, dstW->width);
    int height = std::min(srcW->height, dstW->height);
    if (width <= 0 || height <= 0) return PF_Err_NONE;

    // Read After Effects parameters into PluginParams block
    resoprint::PluginParams p;

    auto checkout_f = [&](int id) -> double {
        PF_ParamDef d;
        AEFX_CLR_STRUCT(d);
        PF_CHECKOUT_PARAM(in_data, id, in_data->current_time, in_data->time_step, in_data->time_scale, &d);
        return d.u.fs_d.value;
    };
    auto checkout_i = [&](int id) -> int {
        PF_ParamDef d;
        AEFX_CLR_STRUCT(d);
        PF_CHECKOUT_PARAM(in_data, id, in_data->current_time, in_data->time_step, in_data->time_scale, &d);
        return (d.param_type == PF_Param_POPUP) ? d.u.pd.value : d.u.sd.value;
    };
    auto checkout_b = [&](int id) -> bool {
        PF_ParamDef d;
        AEFX_CLR_STRUCT(d);
        PF_CHECKOUT_PARAM(in_data, id, in_data->current_time, in_data->time_step, in_data->time_scale, &d);
        return d.u.bd.value != 0;
    };
    auto checkout_c = [&](int id) -> resoprint::ColorRGB {
        PF_ParamDef d;
        AEFX_CLR_STRUCT(d);
        PF_CHECKOUT_PARAM(in_data, id, in_data->current_time, in_data->time_step, in_data->time_scale, &d);
        return { (float)d.u.cd.value.red / 255.0f, (float)d.u.cd.value.green / 255.0f, (float)d.u.cd.value.blue / 255.0f };
    };
    auto checkout_a = [&](int id) -> double {
        PF_ParamDef d;
        AEFX_CLR_STRUCT(d);
        PF_CHECKOUT_PARAM(in_data, id, in_data->current_time, in_data->time_step, in_data->time_scale, &d);
        return (double)d.u.ad.value / 65536.0;
    };

    p.shuffleSeed = checkout_i(ID_SHUFFLE_SEED);
    p.pressPreset = checkout_i(ID_PRESS_PRESET);

    p.plateProof = checkout_i(ID_PLATE_PROOF);
    p.magnifyZoom = (float)checkout_f(ID_MAGNIFY_ZOOM);
    p.magnifyCenterX = (float)checkout_f(ID_MAGNIFY_CENTER_X);
    p.magnifyCenterY = (float)checkout_f(ID_MAGNIFY_CENTER_Y);
    p.cyanPlateEn = checkout_b(ID_CYAN_PLATE_EN);
    p.magentaPlateEn = checkout_b(ID_MAGENTA_PLATE_EN);
    p.yellowPlateEn = checkout_b(ID_YELLOW_PLATE_EN);
    p.blackPlateEn = checkout_b(ID_BLACK_PLATE_EN);
    p.haloHalfRes = checkout_b(ID_HALO_HALF_RES);

    p.machineProfile = checkout_i(ID_MACHINE_PROFILE);
    p.printResolution = checkout_i(ID_PRINT_RESOLUTION);
    p.paperType = checkout_i(ID_PAPER_TYPE);
    p.backgroundMode = checkout_i(ID_BACKGROUND_MODE);
    p.spotInkKit = checkout_i(ID_SPOT_INK_KIT);
    p.workingSpace = checkout_i(ID_WORKING_SPACE);

    p.brightness = (float)checkout_f(ID_BRIGHTNESS);
    p.contrast = (float)checkout_f(ID_CONTRAST);
    p.hue = (float)checkout_f(ID_HUE);
    p.saturation = (float)checkout_f(ID_SATURATION);
    p.hsvValue = (float)checkout_f(ID_HSV_VALUE);
    p.gamma = (float)checkout_f(ID_GAMMA);

    p.colorMode = checkout_i(ID_COLOR_MODE);
    p.spotSeparation = checkout_i(ID_SPOT_SEPARATION);
    p.lineArtPlate = checkout_i(ID_LINE_ART_PLATE);
    p.lineArtThreshold = (float)checkout_f(ID_LINE_ART_THRESH);
    p.lineArtStrength = (float)checkout_f(ID_LINE_ART_STRENGTH);
    p.activePlates = checkout_i(ID_ACTIVE_PLATES);
    p.coverage = (float)checkout_f(ID_COVERAGE);

    p.inkCyan = checkout_c(ID_INK_CYAN);
    p.inkMagenta = checkout_c(ID_INK_MAGENTA);
    p.inkYellow = checkout_c(ID_INK_YELLOW);
    p.inkBlack = checkout_c(ID_INK_BLACK);

    p.spotInk1 = checkout_i(ID_PLATE_1_INK);
    p.spotInk2 = checkout_i(ID_PLATE_2_INK);
    p.spotInk3 = checkout_i(ID_PLATE_3_INK);
    p.spotInk4 = checkout_i(ID_PLATE_4_INK);

    p.blackBoost = (float)checkout_f(ID_BLACK_BOOST);
    p.risoTint = (float)checkout_f(ID_RISO_TINT);
    p.overprintMode = checkout_i(ID_OVERPRINT_MODE);
    p.cyanTranslucency = (float)checkout_f(ID_CYAN_TRANSLUCENCY);
    p.magentaTranslucency = (float)checkout_f(ID_MAGENTA_TRANSLUCENCY);
    p.yellowTranslucency = (float)checkout_f(ID_YELLOW_TRANSLUCENCY);
    p.blackTranslucency = (float)checkout_f(ID_BLACK_TRANSLUCENCY);
    p.totalCoverageLimit = (float)checkout_f(ID_TAC_LIMIT);
    p.inkPooling = (float)checkout_f(ID_INK_POOLING);
    p.haloRadius = (float)checkout_f(ID_HALO_RADIUS);
    p.haloStrength = (float)checkout_f(ID_HALO_STRENGTH);
    p.cyanCurve = (float)checkout_f(ID_CYAN_CURVE);
    p.magentaCurve = (float)checkout_f(ID_MAGENTA_CURVE);
    p.yellowCurve = (float)checkout_f(ID_YELLOW_CURVE);
    p.blackCurve = (float)checkout_f(ID_BLACK_CURVE);

    p.screenType = checkout_i(ID_SCREEN_TYPE);
    p.screenDensity = (float)checkout_f(ID_SCREEN_DENSITY);
    p.dotShape = checkout_i(ID_DOT_SHAPE);
    p.dotHardness = (float)checkout_f(ID_DOT_HARDNESS);
    p.softness = (float)checkout_f(ID_SOFTNESS);
    p.distortion = (float)checkout_f(ID_DISTORTION);
    p.dotStretch = (float)checkout_f(ID_DOT_STRETCH);
    p.edgeSoftness = (float)checkout_f(ID_EDGE_SOFTNESS);
    p.dotGain = (float)checkout_f(ID_DOT_GAIN);
    p.angleCyan = (float)checkout_a(ID_CYAN_ANGLE);
    p.angleMagenta = (float)checkout_a(ID_MAGENTA_ANGLE);
    p.angleYellow = (float)checkout_a(ID_YELLOW_ANGLE);
    p.angleBlack = (float)checkout_a(ID_BLACK_ANGLE);
    p.weightCyan = (float)checkout_f(ID_CYAN_WEIGHT);
    p.weightMagenta = (float)checkout_f(ID_MAGENTA_WEIGHT);
    p.weightYellow = (float)checkout_f(ID_YELLOW_WEIGHT);
    p.weightBlack = (float)checkout_f(ID_BLACK_WEIGHT);
    p.drumAnimSpeed = (float)checkout_f(ID_DRUM_ANIM_SPEED);
    p.richBlacks = (float)checkout_f(ID_RICH_BLACKS);

    p.allOffsetX = (float)checkout_f(ID_ALL_OFFSET_X);
    p.allOffsetY = (float)checkout_f(ID_ALL_OFFSET_Y);
    p.allRotation = (float)checkout_f(ID_ALL_ROTATION);
    p.allSkew = (float)checkout_f(ID_ALL_SKEW);
    p.cyanOffsetX = (float)checkout_f(ID_CYAN_OFFSET_X);
    p.cyanOffsetY = (float)checkout_f(ID_CYAN_OFFSET_Y);
    p.magentaOffsetX = (float)checkout_f(ID_MAGENTA_OFFSET_X);
    p.magentaOffsetY = (float)checkout_f(ID_MAGENTA_OFFSET_Y);
    p.yellowOffsetX = (float)checkout_f(ID_YELLOW_OFFSET_X);
    p.yellowOffsetY = (float)checkout_f(ID_YELLOW_OFFSET_Y);
    p.blackOffsetX = (float)checkout_f(ID_BLACK_OFFSET_X);
    p.blackOffsetY = (float)checkout_f(ID_BLACK_OFFSET_Y);
    p.cyanRotation = (float)checkout_f(ID_CYAN_ROTATION);
    p.magentaRotation = (float)checkout_f(ID_MAGENTA_ROTATION);
    p.yellowRotation = (float)checkout_f(ID_YELLOW_ROTATION);
    p.blackRotation = (float)checkout_f(ID_BLACK_ROTATION);
    p.cyanSkew = (float)checkout_f(ID_CYAN_SKEW);
    p.magentaSkew = (float)checkout_f(ID_MAGENTA_SKEW);
    p.yellowSkew = (float)checkout_f(ID_YELLOW_SKEW);
    p.blackSkew = (float)checkout_f(ID_BLACK_SKEW);

    p.paperColor = checkout_c(ID_PAPER_COLOR);
    p.paperGrain = (float)checkout_f(ID_PAPER_GRAIN);
    p.grainScale = (float)checkout_f(ID_GRAIN_SCALE);
    p.sheetWarp = (float)checkout_f(ID_SHEET_WARP);
    p.agingAmount = (float)checkout_f(ID_AGING_AMOUNT);
    p.agingColor = checkout_c(ID_AGING_COLOR);

    p.sunBleach = (float)checkout_f(ID_SUN_BLEACH);
    p.bleachDirection = (float)checkout_a(ID_BLEACH_DIRECTION);
    p.inkVariance = (float)checkout_f(ID_INK_VARIANCE);
    p.inkFlecks = (float)checkout_f(ID_INK_FLECKS);
    p.fleckScale = (float)checkout_f(ID_FLECK_SCALE);
    p.inkBlots = (float)checkout_f(ID_INK_BLOTS);
    p.rollerStreaks = (float)checkout_f(ID_ROLLER_STREAKS);
    p.streakFreq = (float)checkout_f(ID_STREAK_FREQ);
    p.streakAngle = (float)checkout_a(ID_STREAK_ANGLE);
    p.ghostOffsetX = (float)checkout_f(ID_GHOST_OFFSET_X);
    p.ghostOffsetY = (float)checkout_f(ID_GHOST_OFFSET_Y);
    p.ghostOpacity = (float)checkout_f(ID_GHOST_OPACITY);
    p.inkReticulation = (float)checkout_f(ID_INK_RETICULATION);
    p.reticScale = (float)checkout_f(ID_RETIC_SCALE);
    p.impressions = checkout_i(ID_IMPRESSIONS);
    p.impressionJitter = (float)checkout_f(ID_IMPRESSION_JITTER);
    p.inkDrift = (float)checkout_f(ID_INK_DRIFT);
    p.reversePrint = checkout_b(ID_REVERSE_PRINT);

    p.lightIntensity = (float)checkout_f(ID_LIGHT_INTENSITY);
    p.lightAngle = (float)checkout_a(ID_LIGHT_ANGLE);
    p.lightElevation = (float)checkout_f(ID_LIGHT_ELEVATION);
    p.ambientLight = (float)checkout_f(ID_AMBIENT_LIGHT);
    p.threePointLight = (float)checkout_f(ID_THREE_POINT_LIGHT);

    p.outputGamma = (float)checkout_f(ID_OUTPUT_GAMMA);
    p.outputQuality = checkout_i(ID_OUTPUT_QUALITY);
    p.pressFidelity = (float)checkout_f(ID_PRESS_FIDELITY);
    p.softProof = checkout_i(ID_SOFT_PROOF);
    p.negativeOutput = checkout_b(ID_NEGATIVE_OUTPUT);
    p.blendOriginal = (float)checkout_f(ID_BLEND_ORIGINAL);

    // Allocate intermediate float RGBA buffers
    std::vector<float> srcFloats((size_t)width * height * 4);
    std::vector<float> dstFloats((size_t)width * height * 4);

    // Unpack Source according to bit depth
    if (format == PF_PixelFormat_ARGB128) {
        // 32-bit Float
        for (int y = 0; y < height; ++y) {
            const PF_PixelFloat* srcRow = (const PF_PixelFloat*)((const char*)srcW->data + y * srcW->rowbytes);
            float* dstRow = srcFloats.data() + (size_t)y * width * 4;
            for (int x = 0; x < width; ++x) {
                dstRow[x * 4 + 0] = srcRow[x].red;
                dstRow[x * 4 + 1] = srcRow[x].green;
                dstRow[x * 4 + 2] = srcRow[x].blue;
                dstRow[x * 4 + 3] = srcRow[x].alpha;
            }
        }
    } else if (format == PF_PixelFormat_ARGB64) {
        // 16-bit Integer
        for (int y = 0; y < height; ++y) {
            const PF_Pixel16* srcRow = (const PF_Pixel16*)((const char*)srcW->data + y * srcW->rowbytes);
            float* dstRow = srcFloats.data() + (size_t)y * width * 4;
            for (int x = 0; x < width; ++x) {
                dstRow[x * 4 + 0] = (float)srcRow[x].red / 32768.0f;
                dstRow[x * 4 + 1] = (float)srcRow[x].green / 32768.0f;
                dstRow[x * 4 + 2] = (float)srcRow[x].blue / 32768.0f;
                dstRow[x * 4 + 3] = (float)srcRow[x].alpha / 32768.0f;
            }
        }
    } else {
        // 8-bit Integer (PF_PixelFormat_ARGB32)
        for (int y = 0; y < height; ++y) {
            const PF_Pixel8* srcRow = (const PF_Pixel8*)((const char*)srcW->data + y * srcW->rowbytes);
            float* dstRow = srcFloats.data() + (size_t)y * width * 4;
            for (int x = 0; x < width; ++x) {
                dstRow[x * 4 + 0] = (float)srcRow[x].red / 255.0f;
                dstRow[x * 4 + 1] = (float)srcRow[x].green / 255.0f;
                dstRow[x * 4 + 2] = (float)srcRow[x].blue / 255.0f;
                dstRow[x * 4 + 3] = (float)srcRow[x].alpha / 255.0f;
            }
        }
    }

    resoprint::ImageBuffer srcBuf{ width, height, srcFloats.data(), width * 4 * (int)sizeof(float) };
    resoprint::ImageBuffer dstBuf{ width, height, dstFloats.data(), width * 4 * (int)sizeof(float) };

    double curTimeSec = (in_data->time_scale > 0) ? ((double)in_data->current_time / in_data->time_scale) : 0.0;

    // Run Full CPU Risoprint Processing Engine
    resoprint::RenderRisoprint(srcBuf, dstBuf, p, curTimeSec);

    // Pack Result back into After Effects Output World
    if (format == PF_PixelFormat_ARGB128) {
        for (int y = 0; y < height; ++y) {
            PF_PixelFloat* dstRow = (PF_PixelFloat*)((char*)dstW->data + y * dstW->rowbytes);
            const float* srcRow = dstFloats.data() + (size_t)y * width * 4;
            for (int x = 0; x < width; ++x) {
                dstRow[x].red   = srcRow[x * 4 + 0];
                dstRow[x].green = srcRow[x * 4 + 1];
                dstRow[x].blue  = srcRow[x * 4 + 2];
                dstRow[x].alpha = srcRow[x * 4 + 3];
            }
        }
    } else if (format == PF_PixelFormat_ARGB64) {
        for (int y = 0; y < height; ++y) {
            PF_Pixel16* dstRow = (PF_Pixel16*)((char*)dstW->data + y * dstW->rowbytes);
            const float* srcRow = dstFloats.data() + (size_t)y * width * 4;
            for (int x = 0; x < width; ++x) {
                dstRow[x].red   = (A_u_short)std::round(std::max(0.0f, std::min(1.0f, srcRow[x * 4 + 0])) * 32768.0f);
                dstRow[x].green = (A_u_short)std::round(std::max(0.0f, std::min(1.0f, srcRow[x * 4 + 1])) * 32768.0f);
                dstRow[x].blue  = (A_u_short)std::round(std::max(0.0f, std::min(1.0f, srcRow[x * 4 + 2])) * 32768.0f);
                dstRow[x].alpha = (A_u_short)std::round(std::max(0.0f, std::min(1.0f, srcRow[x * 4 + 3])) * 32768.0f);
            }
        }
    } else {
        for (int y = 0; y < height; ++y) {
            PF_Pixel8* dstRow = (PF_Pixel8*)((char*)dstW->data + y * dstW->rowbytes);
            const float* srcRow = dstFloats.data() + (size_t)y * width * 4;
            for (int x = 0; x < width; ++x) {
                dstRow[x].red   = (A_u_char)std::round(std::max(0.0f, std::min(1.0f, srcRow[x * 4 + 0])) * 255.0f);
                dstRow[x].green = (A_u_char)std::round(std::max(0.0f, std::min(1.0f, srcRow[x * 4 + 1])) * 255.0f);
                dstRow[x].blue  = (A_u_char)std::round(std::max(0.0f, std::min(1.0f, srcRow[x * 4 + 2])) * 255.0f);
                dstRow[x].alpha = (A_u_char)std::round(std::max(0.0f, std::min(1.0f, srcRow[x * 4 + 3])) * 255.0f);
            }
        }
    }

    return PF_Err_NONE;
}

// ---------------------------------------------------------------------------
// Main Plugin Entry Point: EffectMain
// ---------------------------------------------------------------------------

extern "C" DllExport PF_Err EffectMain(
    PF_Cmd cmd,
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_ParamDef* params[],
    PF_LayerDef* output,
    void* extra
) {
    PF_Err err = PF_Err_NONE;
    try {
        switch (cmd) {
            case PF_Cmd_GLOBAL_SETUP:
                err = GlobalSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_PARAMS_SETUP:
                err = ParamsSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_SMART_PRE_RENDER:
                err = PreRender(in_data, out_data, (PF_PreRenderExtra*)extra);
                break;
            case PF_Cmd_SMART_RENDER:
                err = SmartRender(in_data, out_data, (PF_SmartRenderExtra*)extra);
                break;
            default:
                break;
        }
    } catch (...) {
        err = PF_Err_INTERNAL_STRUCT_DAMAGED;
    }
    return err;
}
