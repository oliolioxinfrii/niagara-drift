/* SPDX-License-Identifier: GPL-3.0-only */
/* Experimental SEQ pages: a continuous two-drone score rather than note blocks.
 * Shared pitch scale, amber/cyan paths, little node circles and a playhead.
 * Picture is 240 x 124, the four values remain in the normal bottom band. */
static int32_t sd_y(int cents, int lo, int hi)
{
    return 104 - (int32_t)((int64_t)(cents - lo) * 79 / (hi - lo));
}
static void sd_pitch_text(int g, int cents, char *out)
{
    dr_param_text(DR_GP(g, GP_PITCH), cents, out);
}
/* Each page maps its entire current cycle onto the available width. The
 * horizontal endpoint is the next return to step 1, not the four-minute cap. */
static int32_t sd_time_x(uint32_t seconds, uint32_t total, int32_t x0, int32_t width)
{
    return x0 + (int32_t)((uint64_t)seconds * (uint32_t)width / total);
}
static void lk_pic_seq_time(void)
{
    const int32_t x0 = 18, width = 204, y0 = 55, y1 = 76;
    const uint32_t total = sd_total_s();
    const int32_t x1 = x0 + width;
    const uint32_t elapsed = sd_elapsed_ms();
    int i, g;
    char t[24];

    cv_text(8, 4, &FONT_XS, "EVENT POSITIONS", LK_LABEL);
    if (sd.count < 2) {
        cv_rect(x0, y0 + 9, width, 1, LK_RULE);
        cv_round(x0 - 3, y0 + 6, 7, 7, 3, LK_WHITE);
        for (i = 1; i < SD_MAX; i++) {
            const int32_t x = x0 + width * i / SD_MAX;
            cv_round(x - 3, y0 + 6, 7, 7, 3, LK_UNLIT);
        }
        cv_text(20, 96, &LK_F_TAG, "1 STEP - NO INTERVALS", LK_LABEL);
        return;
    }
    /* A change to the loop time changes the ruler's units, never its endpoints.
     * All active nodes preserve their fractional positions in this picture. */
    for (i = 0; i <= 4; i++) {
        const int32_t x = x0 + width * i / 4;
        cv_rect(x, y0 - 11, 1, 37, LK_RULE);
    }
    for (g = 0; g < 2; g++) {
        const int32_t y = g ? y1 : y0;
        const uint16_t col = LK_DRONE[g];
        cv_rect(x0, y, width + 1, 1, LK_RULE);
        cv_round(x0, y - 1, width + 1, 3, 1, col);
        for (i = 0; i < sd.count; i++) {
            const int32_t x = sd_time_x(sd_node_s(i), total, x0, width);
            cv_round(x - 3, y - 3, 7, 7, 3, col);
            cv_round(x - 1, y - 1, 3, 3, 1, LK_BG);
        }
        if (sd.enabled && ui.playing) {
            const uint32_t tms = elapsed % (total * 1000u);
            const int32_t x = x0 + (int32_t)((uint64_t)tms * (uint32_t)width / (total * 1000u));
            cv_round(x - 3, y - 3, 7, 7, 3, LK_WHITE);
        }
        cv_round(x1 - 3, y - 3, 7, 7, 3, LK_GOLD);
    }
    /* Inactive positions are placeholders off the active paths. */
    for (i = sd.count; i < SD_MAX; i++) {
        const int32_t x = x0 + width * i / SD_MAX;
        cv_round(x - 2, y0 - 16, 5, 5, 2, LK_UNLIT);
        cv_round(x - 2, y1 + 11, 5, 5, 2, LK_UNLIT);
    }
    cv_text(7, 34, &FONT_XS, "I", LK_DRONE[0]);
    cv_text(7, 77, &FONT_XS, "II", LK_DRONE[1]);
    cv_text(x0, 100, &FONT_XS, "0", LK_LABEL);
    itoa_u(total, t);
    {
        char *q = t;
        while (*q) q++;
        scopy(q, "s");
    }
    cv_text(x1 - text_w(&FONT_XS, t), 100, &FONT_XS, t, LK_LABEL);
    text_c(120, 100, &FONT_XS, sd.paused ? "PAUSED" : sd.enabled ? "RUNNING" : "LOOP", LK_LABEL);
}

/* The wave displacement is zero at programmed notes, so the luminous strands
 * retain exact pitch markers while moving between them. The same phase shift
 * used for audio determines drone II's position along the score. */
static float sd_curve_envelope(int g, uint32_t ms)
{
    const uint32_t len = sd_total_s() * 1000u;
    uint32_t t;
    int i;
    if (!len) return 1.0f;
    t = ms % len;
    if (g && !sd_linked())
        t = (t + len * (uint32_t)sd.phase / 100u) % len;
    for (i = 0; i < sd.count; i++) {
        const uint32_t interval = (uint32_t)sd.gap_s[i] * 1000u;
        if (t < interval)
            return fm_sinf(3.1415926f * (float)t / (float)interval);
        t -= interval;
    }
    return 0.0f;
}
static uint32_t sd_curve_node_ms(int g, int pos, uint32_t len)
{
    const uint32_t at = sd_node_s(pos) * 1000u;
    const uint32_t offset = g && !sd_linked() ? len * (uint32_t)sd.phase / 100u : 0u;
    return (at + len - offset) % len;
}
static void lk_pic_sequence(int timing)
{
    const int32_t x0 = 38, width = 187;
    const uint32_t len = sd_total_s() * 1000u;
    const float motion = (float)(plat_ms() % 4000u) * (1.0f / 4000.0f);
    int lo = 32767, hi = -32768, g, j, k;
    char txt[16];
    if (timing) {
        lk_pic_seq_time();
        return;
    }
    for (g = 0; g < 2; g++)
        for (j = 0; j < sd.count; j++) {
            const int n = sd_pitch(g, j);
            if (n < lo) lo = n;
            if (n > hi) hi = n;
        }
    lo = ((lo - 200) / 100 - 2) * 100;
    hi = ((hi + 200) / 100 + 2) * 100;
    if (hi - lo < 1200) {
        lo -= (1200 - (hi - lo)) / 2;
        hi = lo + 1200;
    }
    for (k = 0; k <= 12; k++) {
        const int n = lo + (hi - lo) * k / 12;
        const int32_t y = sd_y(n, lo, hi);
        if (y < 10 || y > 107) continue;
        cv_rect(x0, y, width, 1, LK_RULE);
        cv_rect(x0 - 4, y, 4, 1, LK_UNLIT);
        if (k % 3 == 0) {
            sd_pitch_text(0, n, txt);
            cv_text(2, y - 7, &FONT_XS, txt, LK_LABEL);
        }
    }
    /* Setup 2's animated three-strand waveform, bent around the actual pitch
     * trajectory. Drone II's pitch and node positions share the audio phase. */
    for (g = 1; g >= 0; g--) {
        const float ph = motion + 0.5f * (float)g;
        int strand;
        for (strand = 0; strand < 3; strand++) {
            const uint16_t col = lk_ramp(g, (float)strand / 2.0f);
            const int32_t dy = 3 * (2 - strand);
            int32_t prev = 0;
            int x;
            for (x = 0; x <= width; x++) {
                const uint32_t t = len ? (uint32_t)((uint64_t)len * (uint32_t)x / (uint32_t)width) : 0u;
                const float taper = len ? sd_curve_envelope(g, t) :
                                    fm_sinf(3.1415926f * (float)x / (float)width);
                const float wave = 5.0f * fm_sinf(6.2831853f * ((float)x / 36.5f + ph)) * taper;
                const int32_t y = sd_y(sd_at(g, t), lo, hi) - dy + (int32_t)(wave + (wave >= 0.0f ? 0.5f : -0.5f));
                if (x)
                    cv_line(x0 + x - 1, prev, x0 + x, y, col);
                prev = y;
            }
        }
        for (j = 0; j < sd.count; j++) {
            const uint32_t t = len ? sd_curve_node_ms(g, j, len) : 0u;
            const int32_t x = x0 + (len ? (int32_t)((uint64_t)t * (uint32_t)width / len) : 0);
            const int32_t y = sd_y(sd_pitch(g, j), lo, hi);
            const int selected = j == sd.position && g == (sd_linked() ? 0 : sd.drone);
            cv_round(x - (selected ? 4 : 3), y - (selected ? 4 : 3),
                     selected ? 9 : 7, selected ? 9 : 7, selected ? 4 : 3, selected ? LK_WHITE : LK_DRONE[g]);
            cv_round(x - 1, y - 1, 3, 3, 1, LK_BG);
        }
    }
    if (sd.enabled && ui.playing && len) {
        const uint32_t t = sd_elapsed_ms() % len;
        const int32_t x = x0 + (int32_t)((uint64_t)t * (uint32_t)width / len);
        /* Both dots are at the same clock position, as on Swell page 1.
         * Drone II's phase changes its height on the displaced trace. */
        for (g = 1; g >= 0; g--) {
            const int32_t y = sd_y(sd_at(g, t), lo, hi);
            cv_round(x - 3, y - 3, 7, 7, 3, LK_WHITE);
            cv_round(x - 2, y - 2, 5, 5, 2, LK_DRONE[g]);
        }
    }
    for (j = 0; j < sd.count; j++) {
        const uint32_t t = len ? sd_curve_node_ms(0, j, len) : 0u;
        const int32_t x = x0 + (len ? (int32_t)((uint64_t)t * (uint32_t)width / len) : 0);
        itoa_u((uint32_t)j + 1u, txt);
        cv_text(x - 3, 108, &FONT_XS, txt, j == sd.position ? LK_WHITE : LK_LABEL);
    }
    cv_text(5, 0, &FONT_XS, "PITCH", LK_LABEL);
    if (sd_linked())
        cv_text(130, 0, &FONT_XS, "PITCHES LINKED", LK_LABEL);
    if (sd.enabled)
        cv_text(sd.paused ? 192 : 202, 0, &FONT_XS, sd.paused ? "PAUSE" : "ON", LK_GOLD);
}

/* Values use the same bottom row as the standard 0.3 pages. Inactive timing
 * controls are visibly empty and cannot affect the sequence. */
static void lk_values_sequence(void)
{
    const int timing = ui.view == V_SEQ_TIME;
    uint32_t h = hash(2166136261u, ui.view | (uint32_t)sd.count << 8 | (uint32_t)sd.position << 12 |
                                 (uint32_t)sd.drone << 16 | (uint32_t)sd.phase << 18 |
                                 (uint32_t)sd.enabled << 26);
    int i;
    for (i = 0; i < 4; i++) {
        h = hash(h, (uint32_t)sd.gap_s[i] | ((uint32_t)(uint16_t)sd.note[0][i] << 16));
        h = hash(h, (uint32_t)(uint16_t)sd.note[1][i] | (uint32_t)lit(i) << 20);
    }
    h = hash(h, (uint32_t)(ui.btn >> B_SEL & 1u) | (uint32_t)sd_linked() << 8 |
                (uint32_t)(uint16_t)proj.par[P_PLINK_OFF] << 12);
    if (h == ui.sig[2]) return;
    ui.sig[2] = h;
    cv_begin(240, LK_VAL_H, LK_BG);
    cv_rect(8, 0, 224, 1, LK_RULE);
    for (i = 0; i < 4; i++) {
        const int32_t cx = 36 + 56 * i;
        const int active = !timing ? !(sd_linked() && (i == 1 || i == 3)) :
                           (sd.count > 1 && (i == 3 || i + 1 < sd.count));
        const uint16_t col = !active ? LK_UNLIT : lit(i) ? LK_WHITE :
                             !timing && i == 1 ? LK_DRONE[sd.drone] : LK_NEUTRAL;
        const char *name = timing ? (i == 0 ? "Node 2" : i == 1 ? "Node 3" : i == 2 ? "Node 4" : "Loop") :
                           (i == 0 ? (ui.btn >> B_SEL & 1u ? "Pitch" : "Position") :
                            i == 1 ? "Drone" : i == 2 ? "Steps" : "Phase");
        char value[20];
        if (lit(i) && active) {
            cv_round(cx - 26, 4, 52, 81, 5, LK_DRONE[!timing && i == 1 ? sd.drone : 0]);
            cv_round(cx - 25, 5, 50, 79, 4, LK_BG);
        }
        text_c(cx, 8, &FONT_T20, name, col);
        if (!active) {
            scopy(value, !timing && i == 1 && sd_linked() ? "LINK" : "--");
        } else if (timing) {
            char *q;
            const uint32_t t = i == 3 ? sd_total_s() : sd_node_s(i + 1);
            itoa_u(t, value);
            q = value;
            while (*q) q++;
            scopy(q, "s");
        } else if (i == 0) {
            if (ui.btn >> B_SEL & 1u)
                sd_pitch_text(sd.drone, sd_pitch(sd_linked() ? 0 : sd.drone, sd.position), value);
            else {
                char *q;
                itoa_u((uint32_t)sd.position + 1u, value);
                q = value;
                while (*q) q++;
                *q++ = '/';
                itoa_u(sd.count, q);
            }
        } else if (i == 1) {
            scopy(value, sd.drone ? "II" : "I");
        } else if (i == 2) {
            itoa_u(sd.count, value);
        } else {
            itoa_u(sd.phase, value);
            char *q = value;
            while (*q) q++;
            scopy(q, "%");
        }
        text_c(cx, 39, text_w(&FONT_N1, value) <= 52 ? &FONT_N1 : &FONT_N2,
               value, active ? LK_WHITE : LK_UNLIT);
        if (active && timing) {
            const uint32_t total = sd_total_s();
            const uint32_t value = i == 3 ? total : sd_node_s(i + 1);
            const uint32_t scale = i == 3 ? SD_LIMIT_S : total;
            cv_rect(cx - 22, 72, 44, 1, LK_RULE);
            cv_round(cx - 22 + (int32_t)(value * 43 / scale) - 2, 69, 5, 5, 2, col);
        } else if (active && !timing) {
            const int v = i == 0 ? sd.position : i == 1 ? sd.drone : i == 2 ? sd.count - 1 : sd.phase;
            const int max = i == 0 ? sd.count - 1 : i == 1 ? 1 : i == 2 ? 3 : 100;
            lk_dotrow(cx, 72, max > 0 ? (float)v / (float)max : 0.0f, LK_RAMP[0][0], LK_RAMP[0][1], 0, 0);
        }
    }
    cv_blit(0, LK_VAL_Y);
}
