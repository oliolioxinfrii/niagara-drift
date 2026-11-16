/* SPDX-License-Identifier: GPL-3.0-only */
/* Orbit/scene browser and the transient geological navigation atlas. */

/* Eight bank identities, from P1's FX deep blue to P2's icy white.
 * The SAVE browser navigation order (A-F, P1, P2) is unchanged. */
static uint16_t vs_bank_ink(int bank)
{
    static const uint8_t stop[NBANKS] = {1, 2, 3, 0, 4, 5, 6, 7};
    return lk_mix(LK_FX_RAMP[0], LK_FX_RAMP[1], (float)stop[bank] / 7.0f);
}
/* The ellipse is a dotted instrument track, not a rendered planet orbit.
 * Only the track and node outlines adopt the current bank's shade.
 * The satellite remains the original warm geological palette. */
static void vs_orbit_figure(int bank, int slot, int filled, const char *name)
{
    const float twopi = 6.2831853f, quarter = 1.5707963f;
    const uint16_t bank_ink = vs_bank_ink(bank);
    const uint16_t node_fill = lk_mix(LK_FX_RAMP[0], LK_FX_RAMP[1], 0.0f);
    int i, band;
    cv_text(10, 2, &FONT_XS, "ORBIT / RECORDED STATES", LK_LABEL);
    /* The planetary fragment is intentionally only a layered cutaway. */
    for (band = 5; band >= 0; band--) {
        int lastx = 7;
        int lasty = 97 + band * 7 + (int)(113 * 113 / 340);
        const uint16_t c = vg_dim(VG_STRATA[0][band + 2], 0.65f);
        for (i = 9; i <= 233; i += 2) {
            const int dx = i - 120;
            const int y = 94 + band * 7 + dx * dx / 340 +
                          ((int)(vg_hash((uint32_t)(i / 9 + band * 37)) % 3u) - 1);
            if (y < LK_PIC_H) cv_line(lastx, lasty, i, y, c);
            lastx = i; lasty = y;
        }
    }
    for (i = 0; i < 160; i++) {
        const uint32_t h = vg_hash((uint32_t)(i * 167 + 347));
        const int x = 8 + (int)(h % 224u);
        const int dx = x - 120;
        const int y = 95 + dx * dx / 340 + (int)((h >> 10) % 28u);
        if (y < 123)
            cv_rect(x, y, 1, 1, vg_dim(VG_STRATA[0][2 + i % 6], 0.30f));
    }
    /* Dashed orbital line and eight nodes. */
    for (i = 0; i < 120; i++) {
        const float a = twopi * (float)i / 120.0f;
        const int x = 120 + (int)(101.0f * fm_sinf(a + quarter));
        const int y = 57 + (int)(35.0f * fm_sinf(a));
        if ((i % 5) != 0)
            cv_rect(x, y, 1, 1, vg_dim(bank_ink, 0.80f));
    }
    for (i = 0; i < 8; i++) {
        const float a = twopi * (float)i / 8.0f;
        const int x = 120 + (int)(101.0f * fm_sinf(a + quarter));
        const int y = 57 + (int)(35.0f * fm_sinf(a));
        const uint16_t col = i == slot ? bank_ink : vg_dim(bank_ink, scene_has(bank, i) ? 0.82f : 0.42f);
        cv_round(x - 3, y - 3, 7, 7, 3, col);
        cv_round(x - 1, y - 1, 3, 3, 1, node_fill); /* always deep FX blue, never hollow */
        if (i == slot) {
            cv_rect(x - 1, y - 6, 3, 1, bank_ink);
            cv_rect(x - 1, y + 6, 3, 1, bank_ink);
        }
    }
    /* Two small solar-array grids connected to an angled central body. */
    cv_line(42, 37, 72, 22, VL_SURFACE);
    cv_line(48, 45, 78, 30, VL_SURFACE);
    cv_line(42, 37, 48, 45, VL_SURFACE);
    cv_line(72, 22, 78, 30, VL_SURFACE);
    cv_line(47, 36, 74, 25, VG_STRATA[0][1]);
    for (i = 0; i < 3; i++) {
        cv_line(48 + i * 7, 34 - i * 3, 54 + i * 7, 42 - i * 3,
                vg_dim(VL_SURFACE, 0.75f));
    }
    cv_line(18, 47, 40, 39, VL_SURFACE);
    cv_line(21, 56, 43, 48, VL_SURFACE);
    cv_line(18, 47, 21, 56, VL_SURFACE);
    cv_line(40, 39, 43, 48, VL_SURFACE);
    cv_line(25, 46, 28, 54, VL_SURFACE);
    cv_line(32, 43, 35, 51, VL_SURFACE);
    cv_round(42, 35, 11, 11, 2, LK_WHITE);
    cv_round(44, 37, 7, 7, 2, VG_STRATA[0][2]);
    cv_line(48, 35, 51, 29, LK_WHITE);
    if (name || filled)
        cv_text(12, 105, &FONT_XS, name ? name : "STORED", LK_WHITE);
    else
        cv_text(12, 105, &FONT_XS, "EMPTY SLOT", LK_LABEL);
}
static void vl_save_menu(void)
{
    const int bank = BANK_ORDER[ui.menu_bank], slot = ui.menu_slot;
    const int filled = scene_has(bank, slot);
    const char *name = project_slot_name(bank, slot);
    uint32_t h = hash(hash(2166136261u, (uint32_t)bank | (uint32_t)slot << 8 |
                     (uint32_t)ui.menu_load << 16 | (uint32_t)filled << 24),
                     (uint32_t)ui.menu_pct | (uint32_t)ui.save_menu << 8 | (uint32_t)ui.name_pos << 16);
    char key[12], pct[8];
    int i;
    if (ui.save_menu == 2)
        for (i = 0; i < BANK_NAME_LEN; i++) h = hash(h, (uint8_t)ui.name_draft[i]);
    if (name)
        for (i = 0; name[i]; i++) h = hash(h, (uint8_t)name[i]);
    if (plat_ms() < ui.msg_until) h = hash(h, ui.msg_until);
    if (h == ui.sig[0]) return;
    ui.sig[0] = h;
    scene_name(key, bank, slot);
    itoa_u(ui.menu_pct, pct);
    { char *q = pct; while (*q) q++; scopy(q, "%"); }
    cv_begin(240, LK_BAR_H, LK_BG);
    cv_text(8, 3, &LK_F_PAGE,
            ui.save_menu == 3 ? "OVERWRITE?" :
            ui.save_menu == 2 ? "RENAME" : "MEMORY", LK_WHITE);
    cv_text(188, 8, &FONT_XS, "ORBIT", VL_SURFACE);
    cv_rect(8, 26, 224, 2, VL_SURFACE);
    cv_blit(0, 0);
    cv_begin(240, LK_PIC_H, LK_BG);
    if (ui.save_menu == 1) {
        vs_orbit_figure(bank, slot, filled, name);
    } else if (ui.save_menu == 2) {
        /* The cursor stays inside the fixed-size slot-name buffer. */
        cv_text(12, 9, &FONT_XS, "RENAME RECORDED STATE", LK_LABEL);
        cv_rect(9, 25, 222, 1, LK_RULE);
        for (i = 0; i < BANK_NAME_LEN - 1; i++) {
            const char ch[2] = {ui.name_draft[i], 0};
            const int cx = 20 + i * 20;
            text_c(cx, 46, &FONT_T20, ch, i == ui.name_pos ? LK_WHITE : LK_LABEL);
            if (i == ui.name_pos) cv_rect(cx - 8, 68, 16, 2, VL_SURFACE);
        }
        cv_text(10, 84, &FONT_XS, "K1 CURSOR / K2 CHARACTER", LK_LABEL);
        cv_text(10, 108, &FONT_XS, "SAVE CONFIRM   HOME BACK", LK_WHITE);
    } else {
        /* A confirmation frame uses the same orbital grid, obscured by a
         * clear high-contrast warning panel instead of a pictorial eruption. */
        int i;
        for (i = 8; i <= 232; i += 8)
            cv_rect(i, 10 + (i % 17), 1, 1, vg_dim(VL_SURFACE, 0.20f));
        cv_round(9, 12, 222, 100, 5, VL_CRUST);
        cv_round(11, 14, 218, 96, 4, LK_BG);
        cv_text(22, 20, &FONT_T20, "REPLACE THIS SCENE?", LK_WHITE);
        text_c(120, 54, &FONT_N2, key, LK_WHITE);
        text_c(120, 88, &FONT_XS, name ? name : "EXISTING STATE", LK_LABEL);
    }
    if (plat_ms() < ui.msg_until)
        cv_text(8, 112, &FONT_XS, ui.msg[0], LK_WHITE);
    cv_blit(0, LK_PIC_Y);
    cv_begin(240, LK_VAL_H, LK_BG);
    cv_rect(8, 0, 224, 1, LK_RULE);
    if (ui.save_menu == 1) {
        const int32_t cx[4] = {31, 89, 148, 207};
        const char *const label[4] = {"BANK", "SLOT", "ACTION", ui.menu_load == 2 ? "MORPH" : "TIME"};
        const char *const action[3] = {"SAVE", "LOAD", "TRANS"};
        char short_bank[4] = {BANK_L[bank], bank == BANK_PRESETS ? '1' :
                              bank == BANK_PRESETS2 ? '2' : 0, 0};
        const char *const value[4] = {short_bank, key, action[ui.menu_load],
                                      ui.menu_load == 2 ? pct : ui.menu_load == 1 ? "NOW" : "--"};
        for (i = 0; i < 4; i++) {
            text_c(cx[i], 9, &FONT_XS, label[i], LK_LABEL);
            text_c(cx[i], 34, &FONT_T20, value[i],
                   i == 2 && ui.menu_load == 0 && BANK_IS_PRESETS(bank) ? LK_UNLIT : LK_WHITE);
            cv_rect(cx[i] - 22, 70, 44, 1, LK_RULE);
            cv_rect(cx[i] - 22, 70, i == 3 ? (ui.menu_load == 2 ?
                    (int)(44.0f * (float)ui.menu_pct * 0.01f) : 0) :
                    (i == 2 ? 15 : 7), 2, VL_SURFACE);
        }
        cv_text(10, 75, &FONT_U2, "SAVE: YES / HOME: BACK", LK_LABEL);
    } else if (ui.save_menu == 3) {
        text_c(120, 14, &FONT_XS, "EXISTING SCENE WILL BE LOST", LK_LABEL);
        text_c(64, 41, &FONT_T20, "SAVE: YES", VL_SURFACE);
        text_c(177, 41, &FONT_T20, "HOME: BACK", LK_WHITE);
    } else {
        text_c(120, 13, &FONT_T20, "A-Z   0-9   -   SPACE", LK_LABEL);
        text_c(120, 45, &FONT_XS, "11 CHARACTERS MAX", LK_LABEL);
    }
    cv_blit(0, LK_VAL_Y);
}
static void vs_atlas_terrain(int y0, const float a[5])
{
    /* Cyan denotes the atmosphere ABOVE the surface, not a rock layer.
     * Below the volcano the colors proceed toward the deep mantle. */
    static const uint16_t colors[5] = {VL_SURFACE, VL_CRUST, VL_MANTLE, VL_DEEP, VL_DEEP};
    static const int8_t warp[8] = {2, 0, -6, -3, 3, 6, 0, -2};
    const float brightness[5] = {a[1], a[2], a[3], a[4], a[4] * 0.70f};
    int layer, i, band;
    /* Atmosphere occupies the space over the mountain summit. */
    for (i = 0; i < 175; i++) {
        const uint32_t h = vg_hash((uint32_t)(i * 97 + 113));
        const int y = 34 + (int)(h % 40u);
        const int x = 62 + (int)((h >> 8) % 45u) - 11 + (74 - y) / 7;
        if (x >= 5 && x <= 114)
            cv_rect(x, y - y0, (h & 31u) == 0u ? 2 : 1, 1,
                    vg_dim(i % 9 == 0 ? LK_WHITE : VL_AIR,
                           0.15f + 0.54f * a[0]));
    }
    /* Crater and flanks are BELOW the atmospheric band, on the Surface. */
    cv_line(6, 101 - y0, 45, 91 - y0, vg_dim(colors[0], a[1]));
    cv_line(45, 91 - y0, 67, 78 - y0, vg_dim(colors[0], a[1]));
    cv_line(67, 78 - y0, 83, 90 - y0, vg_dim(colors[0], a[1]));
    cv_line(83, 90 - y0, 118, 104 - y0, vg_dim(colors[0], a[1]));
    cv_line(6, 104 - y0, 46, 95 - y0, vg_dim(VL_CRUST, a[1] * 0.75f));
    cv_line(46, 95 - y0, 67, 82 - y0, vg_dim(VL_CRUST, a[1] * 0.75f));
    cv_line(67, 82 - y0, 84, 94 - y0, vg_dim(VL_CRUST, a[1] * 0.75f));
    cv_line(84, 94 - y0, 118, 108 - y0, vg_dim(VL_CRUST, a[1] * 0.75f));

    /* Four density levels per stratum, below the surface. */
    for (layer = 0; layer < 5; layer++) {
        const int base = 109 + layer * 20;
        const uint16_t main = vg_dim(colors[layer], brightness[layer]);
        for (band = 0; band < 4; band++) {
            int px = 6;
            int py = base + band * 4 - 4 +
                     vg_interpolate(warp, 8, 0, 112) - y0;
            for (i = 9; i <= 117; i += 3) {
                const int fold = vg_interpolate(warp, 8, i - 6, 112);
                const int ny = base + band * 4 + fold - 4 - y0;
                const uint16_t ink = band == 0 ? main :
                                     vg_dim(main, 0.70f - 0.12f * (float)(band - 1));
                cv_line(px, py, i, ny, ink);
                px = i; py = ny;
            }
        }
        for (i = 0; i < 125; i++) {
            const uint32_t h = vg_hash((uint32_t)(i * 193 + layer * 701));
            const int x = 8 + (int)(h % 107u);
            const int yy = base + 1 + (int)((h >> 9) % 18u) - y0;
            const uint16_t ink = vg_dim(main, (h & 7u) == 0u ? 0.65f : 0.29f);
            cv_rect(x, yy, (h & 31u) == 0u ? 2 : 1, 1, ink);
        }
    }
    /* Magma feeder rises through the rocks to the crater, not into the sky. */
    for (i = 0; i < 4; i++) {
        const uint16_t ink = i == 1 ? vg_dim(VL_SURFACE, a[3]) :
                             vg_dim(VL_MANTLE, 0.43f + 0.50f * a[3]);
        cv_line(66 + i, 209 - y0, 67 + i, 80 - y0, ink);
    }
}
/* Navigation remains the original button map: this is a transient observatory
 * overview, not a hidden second menu. Brightness indicates active signal or
 * configured intensity; the labels remain fully legible even at zero output. */
static void vl_atlas(void)
{
    const float f0 = vl_norm(DR_GP(0, GP_LEVEL));
    const float f1 = vl_norm(DR_GP(1, GP_LEVEL));
    float a[5];
    static const uint16_t color[5] = {VL_AIR, VL_SURFACE, VL_CRUST, VL_MANTLE, VL_DEEP};
    static const char *const name[5] = {"ATMOSPHERE", "SURFACE", "CRUST", "MANTLE", "DEEP MANTLE"};
    static const char *const btn[5] = {"FX", "HOME", "SEQ  ARP", "EDIT  ENV", "LFO"};
    int part, row;
    a[0] = fm_clampf(0.22f + 0.78f * fm_maxf(vl_norm(P_DLY_MIX), vl_norm(P_REV_MIX)), 0.22f, 1.0f);
    a[1] = fm_clampf(0.22f + 0.78f * dr_fade * (f0 + f1) * 0.5f, 0.22f, 1.0f);
    a[2] = fm_clampf(0.22f + 0.78f * (proj.par[P_PULSE] ?
                        dr_pulse_wave_for(0, dr_pulse_ph) : sd.enabled ? 0.50f : 0.0f), 0.22f, 1.0f);
    a[3] = fm_clampf(0.22f + 0.78f * (vl_norm(P_CHAR) + vl_norm(P_FEEDBACK) +
                        vl_norm(P_DRIFT)) / 3.0f, 0.22f, 1.0f);
    a[4] = fm_clampf(0.22f + 0.78f * vl_norm(P_MOVE) *
                       (fm_fabsf(dr_lfo[0]) + fm_fabsf(dr_lfo[1])) * 0.5f, 0.22f, 1.0f);
    for (part = 0; part < 2; part++) {
        const int y0 = part * 120;
        cv_begin(240, 120, LK_BG);
        if (!part) {
            cv_text(9, 2, &LK_F_PAGE, "VOLCANO", LK_WHITE);
            cv_text(196, 8, &FONT_XS, "MAP", LK_LABEL);
            cv_rect(8, 28, 224, 1, LK_RULE);
        }
        vs_atlas_terrain(y0, a);
        cv_rect(121, 35 - y0, 1, 161, LK_RULE);
        for (row = 0; row < 5; row++) {
            const int y = 38 + row * 31;
            cv_rect(127, y - y0, 4, 25, vg_dim(color[row], a[row]));
            cv_text(138, y - y0, &FONT_XS, name[row], LK_WHITE);
            cv_text(142, y + 16 - y0, &FONT_U2, btn[row], LK_LABEL);
            if (row < 4)
                cv_rect(138, y + 30 - y0, 94, 1, LK_RULE);
        }
        cv_rect(8, 201 - y0, 224, 1, LK_RULE);
        cv_text(9, 208 - y0, &FONT_XS, "ORBIT / SAVE", VL_SURFACE);
        cv_text(135, 208 - y0, &FONT_XS, "PLANET / GLO", LK_WHITE);
        cv_text(9, 223 - y0, &FONT_XS, "RELEASE HOME TO RETURN", LK_LABEL);
        cv_blit(0, (uint32_t)y0);
    }
}
