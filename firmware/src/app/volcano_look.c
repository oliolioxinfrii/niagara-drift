/* SPDX-License-Identifier: GPL-3.0-only */
/* The permanent color hierarchy. Layer hue identifies the page family,
 * while two distinct drone hues identify simultaneous sources. */
#define VL_AIR      RGB(126, 216, 219)
#define VL_SURFACE  RGB(247, 210, 70)
#define VL_CRUST    RGB(246, 149, 42)
#define VL_MANTLE   RGB(233, 99, 48)
#define VL_DEEP     RGB(207, 75, 63)
#define VL_FAINT    RGB(79, 48, 43)
#define VL_ROCK     RGB(104, 43, 34)

static uint16_t vl_layer_color(int v)
{
    switch (v) {
    case V_FX: case V_FX2: return VL_AIR;
    case V_PLAY: case V_PLAY2: return VL_SURFACE;
    case V_PARTNER: case V_MOVES: case V_SEQ_NOTES:
    case V_SEQ_TIME: case V_SEQ_SHAPE:
    case V_PULSE: case V_SWELL_SHAPE: return VL_CRUST;
    case V_OSC: case V_SOURCE: case V_LIFE: case V_TONE: return VL_MANTLE;
    case V_LFO_WAVES: case V_LFO: case V_LFO2: return VL_DEEP;
    default: return LK_GOLD;
    }
}
/* Preserve the hue while attenuating the RGB565 channel intensities. */
static uint16_t vl_bright(uint16_t c, float f)
{
    int r, g, b;
    f = fm_clampf(f, 0.0f, 1.0f);
    r = (int)((float)(c >> 11 & 31u) * f);
    g = (int)((float)(c >> 5 & 63u) * f);
    b = (int)((float)(c & 31u) * f);
    return (uint16_t)(r << 11 | g << 5 | b);
}
static float vl_norm(int p)
{
    const dr_param_t *d = dr_param_info(p);
    if (d->hi <= d->lo) return 0.0f;
    return fm_clampf((float)(proj.par[p] - d->lo) / (float)(d->hi - d->lo), 0.0f, 1.0f);
}
static void vl_depth_marker(int v)
{
    static const uint16_t colors[5] = {VL_AIR, VL_SURFACE, VL_CRUST, VL_MANTLE, VL_DEEP};
    int active = v == V_FX || v == V_FX2 ? 0 :
                 v == V_PLAY || v == V_PLAY2 ? 1 :
                 v == V_PARTNER || v == V_MOVES || v == V_SEQ_NOTES ||
                 v == V_SEQ_TIME || v == V_SEQ_SHAPE ||
                 v == V_PULSE || v == V_SWELL_SHAPE ? 2 :
                 v == V_LIFE || v == V_TONE || v == V_OSC || v == V_SOURCE ? 3 :
                 v == V_LFO || v == V_LFO2 || v == V_LFO_WAVES ? 4 : -1;
    int i;
    for (i = 0; i < 5; i++)
        cv_rect(229, 3 + i * 4, 6, 2, i == active ? colors[i] : LK_UNLIT);
}
#include "volcano_geometry.c"
#include "volcano_system.c"
