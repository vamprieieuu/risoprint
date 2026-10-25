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

// Parameter IDs must match the EXACT registration sequence in ParamsSetup 1:1.
// PF_ADD_PARAM appends parameters sequentially starting at index 1.
enum {
    ID_INTENSITY = 1,
    ID_BLEND,
    ID_SHOW_SOURCE,
    ID_PAPER,
    ID_INK_CONTRAST,
    ID_SCALE,
    ID_OPACITY,
    ID_SOFTNESS,
    ID_CONTRAST,
    ID_ENABLED_1,
    ID_INK_1,
    ID_SEP_1,
    ID_SHIFT_X_1,
    ID_SHIFT_Y_1,
    ID_ENABLED_2,
    ID_INK_2,
    ID_SEP_2,
    ID_SHIFT_X_2,
    ID_SHIFT_Y_2,
    ID_ENABLED_3,
    ID_INK_3,
    ID_SEP_3,
    ID_SHIFT_X_3,
    ID_SHIFT_Y_3,
    ID_BLEND_ORIGINAL,
    NUM_PARAMS = ID_BLEND_ORIGINAL
};

static const char* BLEND_CHOICES = "Normal|Darken|Multiply|Color Burn|Lighten|Screen|Plus Lighter|Color Dodge|Overlay|Soft Light|Hard Light";

static PF_Err ParamsSetup(PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[], PF_LayerDef* output) {
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

#define ADD(x) do { err = (x); if (err != PF_Err_NONE) return err; } while(0)
    // 1..9: Global & Texture Controls
    ADD(slider("Intensity (%)", 0, 100, 0, 100, 100, 1, ID_INTENSITY));
    ADD(popup("Blend", 11, 1, BLEND_CHOICES, ID_BLEND));
    ADD(check("Show source", false, ID_SHOW_SOURCE));
    ADD(color("Paper", 255, 255, 255, ID_PAPER));
    ADD(slider("Ink contrast", -1, 1, -1, 1, -0.33, 2, ID_INK_CONTRAST));
    ADD(slider("Scale", 0.03, 1, 0.03, 1, 0.21, 2, ID_SCALE));
    ADD(slider("Opacity", 0, 1, 0, 1, 0.65, 2, ID_OPACITY));
    ADD(slider("Softness", 0, 1, 0, 1, 0.50, 2, ID_SOFTNESS));
    ADD(slider("Contrast", 0.25, 3, 0.25, 3, 1.58, 2, ID_CONTRAST));

    // 10..14: Ink Plate 1 (Default Cyan #50D9F1)
    ADD(check("Enabled 1", true, ID_ENABLED_1));
    ADD(color("Ink 1", 80, 217, 241, ID_INK_1));
    ADD(slider("Separation 1", 0, 1, 0, 1, 0.38, 2, ID_SEP_1));
    ADD(slider("Shift X 1", -0.05, 0.05, -0.02, 0.02, 0.003, 3, ID_SHIFT_X_1));
    ADD(slider("Shift Y 1", -0.05, 0.05, -0.02, 0.02, 0.002, 3, ID_SHIFT_Y_1));

    // 15..19: Ink Plate 2 (Default Warm Ochre #BF7700)
    ADD(check("Enabled 2", true, ID_ENABLED_2));
    ADD(color("Ink 2", 191, 119, 0, ID_INK_2));
    ADD(slider("Separation 2", 0, 1, 0, 1, 0.64, 2, ID_SEP_2));
    ADD(slider("Shift X 2", -0.05, 0.05, -0.02, 0.02, 0.000, 3, ID_SHIFT_X_2));
    ADD(slider("Shift Y 2", -0.05, 0.05, -0.02, 0.02, -0.001, 3, ID_SHIFT_Y_2));

    // 20..24: Ink Plate 3 (Default Deep Blue #2900FF)
    ADD(check("Enabled 3", true, ID_ENABLED_3));
    ADD(color("Ink 3", 41, 0, 255, ID_INK_3));
    ADD(slider("Separation 3", 0, 1, 0, 1, 0.39, 2, ID_SEP_3));
    ADD(slider("Shift X 3", -0.05, 0.05, -0.02, 0.02, 0.000, 3, ID_SHIFT_X_3));
    ADD(slider("Shift Y 3", -0.05, 0.05, -0.02, 0.02, -0.003, 3, ID_SHIFT_Y_3));

    // 25: Blend With Original placed at end with default 0% (avoids ghosting/double exposure)
    ADD(slider("Blend With Original (%)", 0, 100, 0, 100, 0, 1, ID_BLEND_ORIGINAL));
#undef ADD

    out_data->num_params = NUM_PARAMS + 1;
    return err;
}

static PF_Err SmartPreRender(PF_InData* in_data, PF_OutData*, PF_PreRenderExtra* extra) {
    if (!extra || !extra->input || !extra->output || !extra->cb) return PF_Err_BAD_CALLBACK_PARAM;
    PF_RenderRequest req = extra->input->output_request;
    PF_CheckoutResult result{};
    PF_Err err = extra->cb->checkout_layer(in_data->effect_ref, 0, 0, &req, in_data->current_time, in_data->time_step, in_data->time_scale, &result);
    extra->output->result_rect = result.result_rect;
    extra->output->max_result_rect = result.max_result_rect;
    return err;
}

static int getPopup(PF_InData* in_data, int id, int d) {
    PF_ParamDef p;
    AEFX_CLR_STRUCT(p);
    if (PF_CHECKOUT_PARAM(in_data, id, in_data->current_time, in_data->time_step, in_data->time_scale, &p) == PF_Err_NONE) {
        int v = p.u.pd.value;
        PF_CHECKIN_PARAM(in_data, &p);
        return v;
    }
    return d;
}

static double getSlider(PF_InData* in_data, int id, double d) {
    PF_ParamDef p;
    AEFX_CLR_STRUCT(p);
    if (PF_CHECKOUT_PARAM(in_data, id, in_data->current_time, in_data->time_step, in_data->time_scale, &p) == PF_Err_NONE) {
        double v = p.u.fs_d.value;
        PF_CHECKIN_PARAM(in_data, &p);
        return v;
    }
    return d;
}

static bool getCheck(PF_InData* in_data, int id, bool d) {
    PF_ParamDef p;
    AEFX_CLR_STRUCT(p);
    if (PF_CHECKOUT_PARAM(in_data, id, in_data->current_time, in_data->time_step, in_data->time_scale, &p) == PF_Err_NONE) {
        bool v = (p.u.bd.value != 0);
        PF_CHECKIN_PARAM(in_data, &p);
        return v;
    }
    return d;
}

static resoprint::Color getColor(PF_InData* in_data, int id, resoprint::Color d) {
    PF_ParamDef p;
    AEFX_CLR_STRUCT(p);
    if (PF_CHECKOUT_PARAM(in_data, id, in_data->current_time, in_data->time_step, in_data->time_scale, &p) == PF_Err_NONE) {
        d.r = (float)p.u.cd.value.red / 255.0f;
        d.g = (float)p.u.cd.value.green / 255.0f;
        d.b = (float)p.u.cd.value.blue / 255.0f;
        PF_CHECKIN_PARAM(in_data, &p);
    }
    return d;
}

static PF_Err SmartRender(PF_InData* in_data, PF_OutData*, PF_SmartRenderExtra* extra) {
    if (!in_data || !extra || !extra->cb || !extra->input) return PF_Err_BAD_CALLBACK_PARAM;
    PF_EffectWorld* srcW = nullptr;
    PF_EffectWorld* dstW = nullptr;
    extra->cb->checkout_layer_pixels(in_data->effect_ref, 0, &srcW);
    extra->cb->checkout_output(in_data->effect_ref, &dstW);
    if (!srcW || !dstW || !srcW->data || !dstW->data) {
        extra->cb->checkin_layer_pixels(in_data->effect_ref, 0);
        return PF_Err_NONE;
    }

    const int W = std::min((int)srcW->width, (int)dstW->width);
    const int H = std::min((int)srcW->height, (int)dstW->height);
    if (W <= 0 || H <= 0) {
        extra->cb->checkin_layer_pixels(in_data->effect_ref, 0);
        return PF_Err_NONE;
    }

    try {
        resoprint::Params p;
        p.intensity = getSlider(in_data, ID_INTENSITY, 100.0) / 100.0;
        p.blend = getPopup(in_data, ID_BLEND, 1);
        p.showSource = getCheck(in_data, ID_SHOW_SOURCE, false);
        p.paper = getColor(in_data, ID_PAPER, {1.0f, 1.0f, 1.0f});
        p.inkContrast = getSlider(in_data, ID_INK_CONTRAST, -0.33);
        p.scale = getSlider(in_data, ID_SCALE, 0.21);
        p.opacity = getSlider(in_data, ID_OPACITY, 0.65);
        p.softness = getSlider(in_data, ID_SOFTNESS, 0.50);
        p.contrast = getSlider(in_data, ID_CONTRAST, 1.58);
        p.blendOriginal = getSlider(in_data, ID_BLEND_ORIGINAL, 0.0) / 100.0;

        p.ink[0].enabled = getCheck(in_data, ID_ENABLED_1, true);
        p.ink[0].color = getColor(in_data, ID_INK_1, {80.0f / 255.0f, 217.0f / 255.0f, 241.0f / 255.0f});
        p.ink[0].separation = getSlider(in_data, ID_SEP_1, 0.38);
        p.ink[0].shiftX = std::max(-0.05, std::min(0.05, getSlider(in_data, ID_SHIFT_X_1, 0.003)));
        p.ink[0].shiftY = std::max(-0.05, std::min(0.05, getSlider(in_data, ID_SHIFT_Y_1, 0.002)));

        p.ink[1].enabled = getCheck(in_data, ID_ENABLED_2, true);
        p.ink[1].color = getColor(in_data, ID_INK_2, {191.0f / 255.0f, 119.0f / 255.0f, 0.0f / 255.0f});
        p.ink[1].separation = getSlider(in_data, ID_SEP_2, 0.64);
        p.ink[1].shiftX = std::max(-0.05, std::min(0.05, getSlider(in_data, ID_SHIFT_X_2, 0.000)));
        p.ink[1].shiftY = std::max(-0.05, std::min(0.05, getSlider(in_data, ID_SHIFT_Y_2, -0.001)));

        p.ink[2].enabled = getCheck(in_data, ID_ENABLED_3, true);
        p.ink[2].color = getColor(in_data, ID_INK_3, {41.0f / 255.0f, 0.0f / 255.0f, 1.0f});
        p.ink[2].separation = getSlider(in_data, ID_SEP_3, 0.39);
        p.ink[2].shiftX = std::max(-0.05, std::min(0.05, getSlider(in_data, ID_SHIFT_X_3, 0.000)));
        p.ink[2].shiftY = std::max(-0.05, std::min(0.05, getSlider(in_data, ID_SHIFT_Y_3, -0.003)));

        // Guard against all-disabled inks or invalid blank color fallbacks
        if (!p.ink[0].enabled && !p.ink[1].enabled && !p.ink[2].enabled) {
            p.ink[0].enabled = p.ink[1].enabled = p.ink[2].enabled = true;
        }

        std::vector<float> sb((size_t)W * H * 4);
        std::vector<float> db((size_t)W * H * 4);
        short bd = extra->input->bitdepth;

        if (bd == 32) {
            for (int y = 0; y < H; y++) {
                const PF_PixelFloat* r = (const PF_PixelFloat*)((const char*)srcW->data + y * srcW->rowbytes);
                float* d = sb.data() + (size_t)y * W * 4;
                for (int x = 0; x < W; x++) {
                    d[x * 4 + 0] = r[x].red;
                    d[x * 4 + 1] = r[x].green;
                    d[x * 4 + 2] = r[x].blue;
                    d[x * 4 + 3] = r[x].alpha;
                }
            }
        } else if (bd == 16) {
            for (int y = 0; y < H; y++) {
                const PF_Pixel16* r = (const PF_Pixel16*)((const char*)srcW->data + y * srcW->rowbytes);
                float* d = sb.data() + (size_t)y * W * 4;
                for (int x = 0; x < W; x++) {
                    d[x * 4 + 0] = (float)r[x].red / 32768.0f;
                    d[x * 4 + 1] = (float)r[x].green / 32768.0f;
                    d[x * 4 + 2] = (float)r[x].blue / 32768.0f;
                    d[x * 4 + 3] = (float)r[x].alpha / 32768.0f;
                }
            }
        } else {
            for (int y = 0; y < H; y++) {
                const PF_Pixel8* r = (const PF_Pixel8*)((const char*)srcW->data + y * srcW->rowbytes);
                float* d = sb.data() + (size_t)y * W * 4;
                for (int x = 0; x < W; x++) {
                    d[x * 4 + 0] = (float)r[x].red / 255.0f;
                    d[x * 4 + 1] = (float)r[x].green / 255.0f;
                    d[x * 4 + 2] = (float)r[x].blue / 255.0f;
                    d[x * 4 + 3] = (float)r[x].alpha / 255.0f;
                }
            }
        }

        resoprint::Image src{W, H, sb.data()};
        resoprint::Image dst{W, H, db.data()};
        resoprint::FrameCtx ctx;
        ctx.timeSec = (in_data->time_scale > 0) ? (double)in_data->current_time / in_data->time_scale : 0.0;

        resoprint::render(src, dst, p, ctx);

        if (bd == 32) {
            for (int y = 0; y < H; y++) {
                PF_PixelFloat* r = (PF_PixelFloat*)((char*)dstW->data + y * dstW->rowbytes);
                const float* d = db.data() + (size_t)y * W * 4;
                for (int x = 0; x < W; x++) {
                    r[x].red = d[x * 4 + 0];
                    r[x].green = d[x * 4 + 1];
                    r[x].blue = d[x * 4 + 2];
                    r[x].alpha = d[x * 4 + 3];
                }
            }
        } else if (bd == 16) {
            for (int y = 0; y < H; y++) {
                PF_Pixel16* r = (PF_Pixel16*)((char*)dstW->data + y * dstW->rowbytes);
                const float* d = db.data() + (size_t)y * W * 4;
                for (int x = 0; x < W; x++) {
                    r[x].red = (A_u_short)(resoprint::clampf(d[x * 4 + 0]) * 32768.0f);
                    r[x].green = (A_u_short)(resoprint::clampf(d[x * 4 + 1]) * 32768.0f);
                    r[x].blue = (A_u_short)(resoprint::clampf(d[x * 4 + 2]) * 32768.0f);
                    r[x].alpha = (A_u_short)(resoprint::clampf(d[x * 4 + 3]) * 32768.0f);
                }
            }
        } else {
            for (int y = 0; y < H; y++) {
                PF_Pixel8* r = (PF_Pixel8*)((char*)dstW->data + y * dstW->rowbytes);
                const float* d = db.data() + (size_t)y * W * 4;
                for (int x = 0; x < W; x++) {
                    r[x].red = (A_u_char)(resoprint::clampf(d[x * 4 + 0]) * 255.0f);
                    r[x].green = (A_u_char)(resoprint::clampf(d[x * 4 + 1]) * 255.0f);
                    r[x].blue = (A_u_char)(resoprint::clampf(d[x * 4 + 2]) * 255.0f);
                    r[x].alpha = (A_u_char)(resoprint::clampf(d[x * 4 + 3]) * 255.0f);
                }
            }
        }
    } catch (...) {
        // Fallback pass-through on exception to avoid crash
        for (int y = 0; y < H; y++) {
            std::memcpy((char*)dstW->data + y * dstW->rowbytes,
                        (const char*)srcW->data + y * srcW->rowbytes,
                        std::min((size_t)dstW->rowbytes, (size_t)srcW->rowbytes));
        }
    }

    extra->cb->checkin_layer_pixels(in_data->effect_ref, 0);
    return PF_Err_NONE;
}

extern "C" DllExport PF_Err EffectMain(PF_Cmd cmd, PF_InData* in_data, PF_OutData* out_data, PF_ParamDef* params[], PF_LayerDef* output, void* extra) {
    PF_Err err = PF_Err_NONE;
    try {
        switch (cmd) {
            case PF_Cmd_ABOUT:
                if (out_data) {
                    std::strncpy(out_data->return_msg, "risoprint 1.0\nNative three-plate risograph / screenprint effect.\n", sizeof(out_data->return_msg) - 1);
                }
                break;
            case PF_Cmd_GLOBAL_SETUP:
                out_data->my_version = PF_VERSION(MAJOR_VERSION, MINOR_VERSION, BUG_VERSION, STAGE_VERSION, BUILD_VERSION);
                out_data->out_flags = 0x02008004;   // DEEP_COLOR_AWARE | NON_PARAM_VARY | CUSTOM_UI
                out_data->out_flags2 = 0x08001400;  // SUPPORTS_SMART_RENDER | FLOAT_COLOR_AWARE
                break;
            case PF_Cmd_PARAMS_SETUP:
                err = ParamsSetup(in_data, out_data, params, output);
                break;
            case PF_Cmd_SMART_PRE_RENDER:
                err = SmartPreRender(in_data, out_data, (PF_PreRenderExtra*)extra);
                break;
            case PF_Cmd_SMART_RENDER:
                err = SmartRender(in_data, out_data, (PF_SmartRenderExtra*)extra);
                break;
            default:
                break;
        }
    } catch (...) {
        err = PF_Err_NONE;
    }
    return err;
}

extern "C" DllExport int PluginDataEntryFunction2(void*, void*, int, int) {
    return 0;
}
