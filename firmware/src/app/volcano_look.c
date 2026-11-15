/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * VOLCANO v1.5 geological drawings. Included by look.c in the existing
 * strip-rendered UI unity build. No allocation, full-screen framebuffer,
 * sound engine changes, or trigonometric work inside the audio ISR.
 *
 * The five colors denote page families. The independent red -> yellow
 * ramp denotes parameter magnitude; numbers and ordinary labels stay pale.
 */
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

/* An RGB565 brightness adjustment, independent of hue and from a black floor. */
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
    if (!d || d->hi <= d->lo) return 0.0f;
    return fm_clampf((float)(proj.par[p] - d->lo) / (float)(d->hi - d->lo), 0.0f, 1.0f);
}
static void vl_depth_marker(int v)
{
    uint16_t col[5] = {VL_AIR, VL_SURFACE, VL_CRUST, VL_MANTLE, VL_DEEP};
    int selected = v == V_FX || v == V_FX2 ? 0 :
                   v == V_PLAY || v == V_PLAY2 ? 1 :
                   v == V_PARTNER || v == V_MOVES || v == V_SEQ_NOTES ||
                   v == V_SEQ_TIME || v == V_SEQ_SHAPE ||
                   v == V_PULSE || v == V_SWELL_SHAPE ? 2 :
                   v == V_LIFE || v == V_TONE || v == V_OSC || v == V_SOURCE ? 3 :
                   v == V_LFO || v == V_LFO2 || v == V_LFO_WAVES ? 4 : -1;
    int i;
    for (i = 0; i < 5; i++)
        cv_rect(229, 3 + i * 4, 6, 2, i == selected ? col[i] : LK_UNLIT);
}

/* An irregular piecewise-linear stratigraphic interface: 9 samples, not a waveform. */
static void vl_stratum(int x, int y, int w, int seed, int buckle, uint16_t ink)
{
    static const int8_t TERRAIN[12] = {0, 1, -2, 1, -4, -1, 3, 0, -3, 1, 2, 0};
    int i, px = x, py = y + (int)TERRAIN[seed % 12] * buckle / 5;
    for (i = 1; i <= 12; i++) {
        int nx = x + w * i / 12;
        int ny = y + (int)TERRAIN[(i + seed) % 12] * buckle / 5;
        cv_line(px, py, nx, ny, ink);
        px = nx;
        py = ny;
    }
}
static void vl_surface(int pitch)
{
    int g, j;
    cv_rect(119, 5, 1, LK_PIC_H - 11, LK_RULE);
    for (g = 0; g < 2; g++) {
        const int x = 9 + 120 * g, width = 101;
        const float gain = vl_norm(DR_GP(g, GP_LEVEL));
        const float cut = vl_norm(DR_GP(g, GP_CUTOFF));
        const float res = vl_norm(DR_GP(g, GP_RESO));
        const float freq = fm_clampf((dr_pitch_live[g] + 24.0f) / 120.0f, 0.0f, 1.0f);
        const int buckle = pitch ? 3 + (int)(res * 11.0f) : 4 + (int)(cut * 9.0f);
        const int shift = pitch ? (int)((0.5f - freq) * 28.0f) : (int)((0.5f - cut) * 23.0f);
        const uint16_t color[5] = {VL_SURFACE, VL_CRUST, VL_MANTLE, VL_DEEP, VL_ROCK};
        cv_text(x + 1, 2, &LK_F_DRONE, g ? "II" : "I", g ? VL_CRUST : VL_SURFACE);
        for (j = 0; j < 5; j++) {
            float strength = 0.45f + 0.55f * gain;
            uint16_t ink = vl_bright(color[j], strength);
            vl_stratum(x, 39 + shift + j * 12, width, j + g * 3, buckle, ink);
        }
        cv_rect(x, 108, (int)(width * gain), 2, g ? VL_CRUST : VL_SURFACE);
        cv_rect(x, 108, width, 1, LK_RULE);
        if (pitch) {
            char note[16];
            dr_param_text(DR_GP(g, GP_PITCH), gpar(g, GP_PITCH), note);
            cv_text(x + 2, 87, &FONT_XS, note, LK_WHITE);
        }
    }
}
/* Two compositional strata representing each drone's oscillator and subharmonic. */
static void vl_mantle_sources(int sub)
{
    int g, j;
    cv_rect(119, 5, 1, 111, LK_RULE);
    for (g = 0; g < 2; g++) {
        const int x = 10 + 120 * g, width = 99;
        const float wave = vl_norm(DR_GP(g, GP_WAVE));
        const float mix = vl_norm(DR_GP(g, GP_MIX));
        const float drive = vl_norm(DR_GP(g, GP_DRIVE));
        const uint16_t col[5] = {VL_SURFACE, VL_CRUST, VL_MANTLE, VL_DEEP, VL_ROCK};
        cv_text(x, 3, &LK_F_DRONE, g ? "II" : "I", g ? VL_MANTLE : VL_CRUST);
        for (j = 0; j < 5; j++) {
            const int buckle = sub ? 4 + (int)(drive * 13.0f) : 3 + (int)(wave * 12.0f);
            const int rise = sub ? (int)(mix * 24.0f) : (int)(wave * 20.0f);
            vl_stratum(x, 36 + 13 * j - rise / 3, width, j + 2 * g + 2, buckle,
                       vl_bright(col[j], 0.58f + 0.42f * (sub ? mix : wave)));
        }
        if (sub) { /* a narrow incandescent intrusion, not an audio scope */
            int hot = 2 + (int)(drive * 23.0f);
            cv_line(x + 51, 112, x + 51 - hot / 2, 46, vl_bright(VL_SURFACE, 0.3f + drive * 0.7f));
            cv_line(x + 52, 112, x + 53 + hot / 2, 46, VL_MANTLE);
        }
    }
}

/* LIFE: one volcanic system, four visually independent observables.
 * Feedback = plume fullness, Drift = displacement, Character = incandescence
 * at the vent, Age = weathering and ash particles. The values are displayed
 * unchanged by the shared knob row beneath this drawing. */
static void vl_life(void)
{
    const float fb = vl_norm(P_FEEDBACK), drift = vl_norm(P_DRIFT);
    const float age = vl_norm(P_AGE), ch = vl_norm(P_CHAR);
    const int blow = (int)(drift * 42.0f);
    const int fullness = 1 + (int)(fb * 6.0f);
    int i, k;
    cv_text(10, 3, &FONT_XS, "VENT / PLUME OBSERVATORY", LK_LABEL);
    /* The cone and its two exposed strata. */
    cv_line(16, 118, 91, 75, VL_CRUST);
    cv_line(91, 75, 118, 83, VL_SURFACE);
    cv_line(118, 83, 221, 118, VL_CRUST);
    cv_line(26, 119, 94, 89, VL_MANTLE);
    cv_line(94, 89, 121, 99, VL_MANTLE);
    cv_line(121, 99, 212, 119, VL_MANTLE);
    cv_line(93, 75, 104, 114, VL_DEEP);
    cv_line(100, 81, 120, 116, VL_MANTLE);
    /* Curving ascent, with stronger crosswind as Drift grows. */
    for (k = 0; k < fullness; k++) {
        int px = 106 + k * 2 - fullness, py = 73;
        for (i = 1; i <= 9; i++) {
            int nx = 105 + k * 2 - fullness + blow * i / 9 +
                     ((i * i + k * 3) % 7 - 3) * (1 + (int)(drift * 2.0f));
            int ny = 73 - i * 7;
            cv_line(px, py, nx, ny, k & 1 ? VL_MANTLE : VL_CRUST);
            px = nx;
            py = ny;
        }
    }
    /* At high Character the vent spits short, bright jets of lava. */
    for (i = 0; i < (int)(ch * 10.0f); i++) {
        int x = 103 + (i * 11 % 27) - 13, y = 75 - (i * 13 % 26);
        cv_line(103, 79, x, y, i & 1 ? VL_SURFACE : VL_CRUST);
        cv_rect(x, y, 2, 2, VL_SURFACE);
    }
    /* Stable, deterministic ash grains; amount depends on Age. */
    for (i = 0; i < (int)(age * 65.0f); i++) {
        int x = 18 + (i * 71 + 11) % 205;
        int y = 18 + (i * 37 + 7) % 95;
        cv_rect(x, y, 1, 1, i & 1 ? LK_LABEL : VL_FAINT);
    }
}
static void vl_tone(void)
{
    int g, i;
    cv_rect(119, 5, 1, 112, LK_RULE);
    for (g = 0; g < 2; g++) {
        const int x = 10 + 120 * g;
        const float noise = vl_norm(DR_GP(g, GP_NOISE));
        const float color = vl_norm(DR_GP(g, GP_NCOLOR));
        const int count = (int)(noise * 115.0f);
        cv_text(x, 3, &LK_F_SMALL, g ? "ASH II" : "ASH I", LK_WHITE);
        cv_line(x + 4, 98, x + 94, 98, VL_ROCK);
        cv_line(x + 30, 98, x + 58, 32, g ? VL_MANTLE : VL_CRUST);
        cv_line(x + 58, 32, x + 87, 98, g ? VL_MANTLE : VL_CRUST);
        for (i = 0; i < count; i++) {
            const int xx = x + 8 + (i * 47 + 9) % 90;
            const int spread = 12 + (int)((1.0f - color) * 72.0f);
            const int yy = 17 + (i * 29 + (i / 3) * 7) % spread;
            cv_rect(xx, yy, 1 + (color > 0.6f && i % 12 == 0), 1,
                    i % 5 == 0 ? VL_SURFACE : (i & 1 ? VL_MANTLE : LK_LABEL));
        }
        cv_text(x, 105, &FONT_XS, color > 0.5f ? "PINK / HEAVY" : "WHITE / FINE", LK_LABEL);
    }
}
/* Minimal plume for both FX pages: wetness sets density, delay sets the
 * separation of echoes, reverb length sets vertical reach. */
static void vl_air(void)
{
    const float wet = fm_maxf(vl_norm(P_REV_MIX), vl_norm(P_DLY_MIX));
    const float delay = vl_norm(P_DLY_REPEATS), length = vl_norm(P_REV_LEN);
    int i, k, h = 30 + (int)(length * 44.0f);
    cv_text(10, 2, &FONT_XS, "VOLCANIC PLUME", LK_LABEL);
    cv_line(10, 112, 94, 87, VL_DEEP);
    cv_line(94, 87, 114, 104, VL_MANTLE);
    cv_line(114, 104, 232, 114, VL_DEEP);
    cv_line(89, 87, 105, 120, VL_CRUST);
    for (k = 0; k < 1 + (int)(wet * 5.0f); k++) {
        int px = 100 + 2 * k, py = 87;
        for (i = 1; i <= 12; i++) {
            const int nx = 100 + 2 * k + 13 * i / 12 + ((i * 5 + k * 3) % 12 - 6) * 2;
            const int ny = 87 - (h * i / 12);
            cv_line(px, py, nx, ny, k & 1 ? VL_MANTLE : VL_CRUST);
            px = nx;
            py = ny;
        }
    }
    for (i = 0; i < (int)(wet * 23.0f + delay * 20.0f); i++) {
        int x = 145 + i * 37 % 87, y = 13 + i * 31 % 66;
        cv_rect(x, y, 1 + (i % 11 == 0), 1, i & 1 ? VL_AIR : vl_bright(VL_AIR, 0.65f));
    }
    if (proj.par[P_REV_LEN] >= 100)
        cv_text(170, 3, &FONT_XS, "FREEZE", VL_AIR);
}
/* Small planetary cutaway, deliberately leaving the right of the page free. */
static void vl_planet(void)
{
    const int cx = 60, cy = 65;
    int k;
    uint16_t c[4] = {VL_DEEP, VL_MANTLE, VL_CRUST, VL_SURFACE};
    for (k = 0; k < 4; k++)
        lk_circle(cx, cy, 44 - k * 10, c[k]);
    cv_line(cx, cy - 46, cx, cy + 46, LK_WHITE);
    cv_line(cx - 47, cy, cx + 47, cy, LK_RULE);
    cv_text(123, 13, &FONT_T20, "PLANET", LK_WHITE);
    cv_rect(123, 40, 106, 1, LK_GOLD);
    cv_text(123, 54, &LK_F_SMALL,
            ui.view == V_SETUP ? "TIME / SYNC" :
            ui.view == V_SETUP2 ? "TUNING" : "SPEAKER", LK_WHITE);
    cv_text(123, 83, &FONT_XS,
            ui.view == V_SETUP ? "GLOBAL CLOCK" :
            ui.view == V_SETUP2 ? "REFERENCE" : "OUTPUT", LK_LABEL);
}
/* SAVE: the memory satellite is an instrument, not a literal terrain map. */
static void vl_save_menu(void)
{
    const int bank = BANK_ORDER[ui.menu_bank], slot = ui.menu_slot;
    const int filled = scene_has(bank, slot);
    const char *name = project_slot_name(bank, slot);
    char key[12], pct[8];
    uint32_t h = hash(hash(2166136261u, (uint32_t)bank | (uint32_t)slot << 8 |
                              (uint32_t)ui.menu_load << 16 | (uint32_t)filled << 24),
                      (uint32_t)ui.menu_pct | (uint32_t)ui.save_menu << 8 | (uint32_t)ui.name_pos << 16);
    int i;
    if (ui.save_menu == 2)
        for (i = 0; i < BANK_NAME_LEN; i++) h = hash(h, (uint8_t)ui.name_draft[i]);
    if (name)
        for (i = 0; name[i]; i++) h = hash(h, (uint8_t)name[i]);
    if (plat_ms() < ui.msg_until) h = hash(h, ui.msg_until);
    if (ui.sig[0] == h) return;
    ui.sig[0] = h;
    scene_name(key, bank, slot);
    itoa_u(ui.menu_pct, pct);
    { char *q = pct; while (*q) q++; scopy(q, "%"); }
    cv_begin(240, LK_BAR_H, LK_BG);
    cv_text(8, 3, &LK_F_PAGE, ui.save_menu == 3 ? "OVERWRITE" :
             ui.save_menu == 2 ? "RENAME" : "MEMORY", LK_GOLD);
    cv_text(181, 8, &FONT_XS, "ORBIT", LK_LABEL);
    cv_rect(8, LK_BAR_H - 2, 224, 2, LK_GOLD);
    cv_blit(0, 0);
    cv_begin(240, LK_PIC_H, LK_BG);
    if (ui.save_menu == 1) {
        /* Elliptical orbit and eight recalled scene nodes. */
        int px = 12, py = 64;
        for (i = 1; i <= 48; i++) {
            float a = 6.2831853f * (float)i / 48.0f;
            int nx = 120 + (int)(102.0f * fm_sinf(a + 1.5707963f));
            int ny = 67 + (int)(32.0f * fm_sinf(a));
            if (i > 1) cv_line(px, py, nx, ny, LK_UNLIT);
            px = nx; py = ny;
        }
        /* Abstract hemispherical magma strata, cropped by the picture. */
        for (i = 0; i < 4; i++)
            vl_stratum(23, 92 + 10 * i, 193, i * 2, 2,
                       (uint16_t[]){VL_SURFACE, VL_CRUST, VL_MANTLE, VL_DEEP}[i]);
        /* Eight scene markers: the selected one glows yellow. */
        for (i = 0; i < 8; i++) {
            const int x = 22 + i * 28, y = 68 - (int)(28.0f * fm_sinf(3.14159265f * (float)i / 7.0f));
            cv_round(x - 2, y - 2, 5, 5, 2, i == slot ? VL_SURFACE : LK_UNLIT);
        }
        /* Minimal satellite; no Earth outline or geography. */
        cv_rect(41, 38, 16, 10, VL_CRUST);
        cv_rect(26, 40, 13, 6, VL_SURFACE);
        cv_rect(59, 40, 13, 6, VL_SURFACE);
        cv_line(49, 38, 52, 30, LK_WHITE);
        cv_text(12, 106, &FONT_XS, name ? name : filled ? "STORED" : "EMPTY", LK_WHITE);
    } else if (ui.save_menu == 2) {
        cv_text(12, 9, &FONT_XS, "EDIT SLOT NAME", LK_LABEL);
        for (i = 0; i < BANK_NAME_LEN - 1; i++) {
            const char ch[2] = {ui.name_draft[i], 0};
            const int x = 18 + 20 * i;
            text_c(x, 41, &FONT_T20, ch, i == ui.name_pos ? VL_SURFACE : LK_WHITE);
            if (i == ui.name_pos) cv_rect(x - 7, 66, 14, 2, VL_SURFACE);
        }
        cv_text(11, 86, &FONT_XS, "K1: CURSOR   K2: LETTER", LK_LABEL);
        cv_text(11, 108, &FONT_XS, "SAVE: YES   HOME: BACK", LK_LABEL);
    } else {
        text_c(120, 14, &FONT_T20, "REPLACE STORED SCENE?", VL_SURFACE);
        text_c(120, 47, &FONT_N2, key, LK_WHITE);
        text_c(120, 85, &FONT_T20, name ? name : "EXISTING SOUND", LK_LABEL);
    }
    if (plat_ms() < ui.msg_until)
        cv_text(8, 111, &FONT_XS, ui.msg[0], LK_GOLD);
    cv_blit(0, LK_PIC_Y);
    cv_begin(240, LK_VAL_H, LK_BG);
    cv_rect(8, 0, 224, 1, LK_RULE);
    if (ui.save_menu == 1) {
        const int32_t cx[4] = {31, 89, 148, 207};
        const char *const label[4] = {"BANK", "SLOT", "ACTION", "MORPH"};
        const char *const action[3] = {"SAVE", "LOAD", "TRANS"};
        char short_bank[4] = {BANK_L[bank], bank == BANK_PRESETS ? '1' : bank == BANK_PRESETS2 ? '2' : 0, 0};
        const char *const value[4] = {short_bank, key, action[ui.menu_load], pct};
        for (i = 0; i < 4; i++) {
            text_c(cx[i], 10, &FONT_XS, label[i], LK_LABEL);
            text_c(cx[i], 32, &FONT_T20, value[i],
                i == 2 && ui.menu_load == 0 && BANK_IS_PRESETS(bank) ? LK_UNLIT : LK_WHITE);
            cv_rect(cx[i] - 24, 68, 48, 2, i == 2 ? VL_CRUST : LK_RULE);
        }
        cv_text(8, 78, &FONT_XS, "SAVE: YES   HOME: BACK", LK_LABEL);
    } else if (ui.save_menu == 3) {
        text_c(120, 12, &FONT_XS, "EXISTING SCENE WILL BE LOST", LK_LABEL);
        text_c(65, 39, &FONT_T20, "SAVE: YES", VL_SURFACE);
        text_c(176, 39, &FONT_T20, "HOME: BACK", LK_WHITE);
    } else {
        text_c(120, 13, &FONT_T20, "A-Z   0-9   -   SPACE", LK_LABEL);
        text_c(120, 44, &FONT_XS, "11 CHARACTERS MAX", LK_LABEL);
    }
    cv_blit(0, LK_VAL_Y);
}

/* Activity map: a diagnostic, not a new navigation hierarchy.
 * HOME held >400 ms temporarily displays the model, release restores page.
 * All labels remain legible even when a particular layer is nearly inactive. */
static void vl_atlas(void)
{
    static const uint16_t C[5] = {VL_AIR, VL_SURFACE, VL_CRUST, VL_MANTLE, VL_DEEP};
    static const char *const N[5] = {"ATMOSPHERE", "SURFACE", "CRUST", "MANTLE", "DEEP MANTLE"};
    static const char *const B[5] = {"FX", "HOME", "SEQ / ARP", "EDIT / ENV", "LFO"};
    float amt[5];
    int part, k;
    amt[0] = fm_clampf(0.15f + 0.85f * fm_maxf(vl_norm(P_DLY_MIX), vl_norm(P_REV_MIX)), 0.15f, 1.0f);
    amt[1] = fm_clampf(0.16f + 0.84f * dr_fade *
                      (vl_norm(DR_GP(0, GP_LEVEL)) + vl_norm(DR_GP(1, GP_LEVEL))) * 0.5f, 0.16f, 1.0f);
    amt[2] = fm_clampf(0.16f + 0.84f *
                      (proj.par[P_PULSE] ? dr_pulse_wave_for(0, dr_pulse_ph) :
                       sd.enabled ? 0.50f : 0.0f), 0.16f, 1.0f);
    amt[3] = fm_clampf(0.16f + 0.84f * (vl_norm(P_CHAR) +
                      vl_norm(P_FEEDBACK) + vl_norm(P_DRIFT)) / 3.0f, 0.16f, 1.0f);
    amt[4] = fm_clampf(0.16f + 0.84f * vl_norm(P_MOVE) *
                      (fm_fabsf(dr_lfo[0]) + fm_fabsf(dr_lfo[1])) * 0.5f, 0.16f, 1.0f);
    for (part = 0; part < 2; part++) {
        const int y0 = part * 120;
        cv_begin(240, 120, LK_BG);
        if (!part) {
            cv_text(8, 3, &LK_F_PAGE, "VOLCANO", LK_WHITE);
            cv_text(148, 8, &FONT_XS, "INSTRUMENT MAP", LK_LABEL);
            cv_rect(8, 28, 224, 1, LK_RULE);
        }
        /* Left: a schematic volcanic cross section, coded by the same five
         * layer intensities as the menu's right-hand reading. */
        cv_line(6, 67 - y0, 55, 43 - y0, vl_bright(VL_SURFACE, amt[1]));
        cv_line(55, 43 - y0, 112, 75 - y0, vl_bright(VL_SURFACE, amt[1]));
        for (k = 0; k < 5; k++) {
            const int gy = 78 + 26 * k;
            cv_line(4, gy - y0, 55, gy - 10 - y0, vl_bright(C[k], amt[k]));
            cv_line(55, gy - 10 - y0, 114, gy + 4 - y0, vl_bright(C[k], amt[k]));
            cv_line(4, gy + 4 - y0, 113, gy + 15 - y0, vl_bright(C[k], amt[k] * 0.65f));
        }
        for (k = 0; k < 5; k++) { /* no text needs color to be readable */
            const int gy = 32 + k * 27;
            cv_rect(121, gy - y0, 4, 21, vl_bright(C[k], amt[k]));
            cv_text(134, gy - y0, &FONT_XS, N[k], LK_WHITE);
            cv_text(134, gy + 12 - y0, &FONT_XS, B[k], LK_LABEL);
        }
        cv_rect(122, 169 - y0, 111, 1, LK_RULE);
        cv_text(124, 174 - y0, &FONT_XS, "ORBIT", LK_GOLD);
        cv_text(189, 174 - y0, &FONT_XS, "SAVE", LK_WHITE);
        cv_text(124, 192 - y0, &FONT_XS, "PLANET", LK_GOLD);
        cv_text(189, 192 - y0, &FONT_XS, "GLO", LK_WHITE);
        cv_rect(8, 214 - y0, 224, 1, LK_RULE);
        cv_text(9, 220 - y0, &FONT_XS, "HOLD HOME: MAP", LK_LABEL);
        cv_text(150, 220 - y0, &FONT_XS, "RELEASE: RETURN", LK_LABEL);
        cv_blit(0, (uint32_t)y0);
    }
}
