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
/* Timing edit: the note paths deliberately give way to one proportional time
 * axis. Inactive positions remain as grey placeholders; only active intervals
 * illuminate. The right-hand marker is LOOP, not an extra pitched step. */
static void lk_pic_seq_time(void)
{
    const int32_t x0 = 18, x1 = 222, y0 = 56, y1 = 75;
    const uint32_t total = sd_total_s();
    uint32_t now = plat_ms();
    int i, g;
    char t[20];
    cv_text(8, 4, &FONT_XS, "EVENT POSITIONS", LK_LABEL);
    if (sd.count < 2) {
        cv_rect(x0, y0 + 7, x1 - x0, 1, LK_RULE);
        for (i = 1; i <= 3; i++) {
            int32_t x = x0 + (x1 - x0) * i / 4;
            cv_round(x - 3, y0 + 4, 7, 7, 3, LK_UNLIT);
        }
        cv_round(x0 - 4, y0 + 3, 9, 9, 4, LK_WHITE);
        cv_text(20, 96, &LK_F_TAG, "1 STEP  -  NO INTERVALS", LK_LABEL);
        return;
    }
    /* A subdued ruler runs the complete four-minute range. Keeping the
     * endpoint proportional makes actual gap changes visible immediately. */
    for (i = 0; i <= 4; i++) {
        const int32_t x = x0 + (x1 - x0) * i / 4;
        cv_rect(x, y0 - 9, 1, 34, LK_RULE);
    }
    for (g = 0; g < 2; g++) {
        const int32_t y = g ? y1 : y0;
        const uint16_t c = LK_DRONE[g];
        const int32_t endx = x0 + (int32_t)((x1 - x0) * total / SD_LIMIT_S);
        cv_rect(x0, y, x1 - x0, 1, LK_RULE);
        cv_round(x0, y - 1, endx - x0 + 1, 3, 1, c);
        for (i = 0; i < sd.count; i++) {
            const uint32_t event_s = sd_node_s(i);
            const int32_t x = x0 + (int32_t)((x1 - x0) * event_s / SD_LIMIT_S);
            cv_round(x - 3, y - 3, 7, 7, 3, c);
            cv_round(x - 1, y - 1, 3, 3, 1, LK_BG);
        }
        if (sd.enabled && ui.playing) {
            const uint32_t tms = (now - sd.started_ms) % (total * 1000u);
            const int32_t x = x0 + (int32_t)((x1 - x0) * tms / (SD_LIMIT_S * 1000u));
            cv_round(x - 3, y - 3, 7, 7, 3, LK_WHITE);
        }
        cv_round(endx - 3, y - 3, 7, 7, 3, LK_GOLD);
    }
    for (i = sd.count; i < SD_MAX; i++) {
        /* Future steps are ghosts at equally spaced positions, not events. */
        const int32_t x = x0 + (x1 - x0) * i / SD_MAX;
        cv_round(x - 2, y0 - 2, 5, 5, 2, LK_UNLIT);
        cv_round(x - 2, y1 - 2, 5, 5, 2, LK_UNLIT);
    }
    cv_text(7, 28, &FONT_XS, "I", LK_DRONE[0]);
    cv_text(7, 47, &FONT_XS, "II", LK_DRONE[1]);
    cv_text(8, 98, &FONT_XS, "0", LK_LABEL);
    cv_text(196, 98, &FONT_XS, "4m", LK_LABEL);
    itoa_u(total, t);
    {
        char *q = t;
        while (*q) q++;
        scopy(q, "s LOOP");
    }
    cv_text(110 - text_w(&LK_F_TAG, t) / 2, 98, &LK_F_TAG, t, LK_WHITE);
}
static void lk_pic_sequence(int timing)
{
    if (timing) {
        lk_pic_seq_time();
        return;
    }
    int lo = 32767, hi = -32768, g, j, k;
    const int32_t x0 = 38, width = 187;
    uint32_t span[SD_MAX], total = 0;
    int32_t px[SD_MAX + 1];
    char txt[16];
    for (g = 0; g < 2; g++)
        for (j = 0; j < sd.count; j++) {
            const int n = sd_pitch(g, j);
            if (n < lo) lo = n;
            if (n > hi) hi = n;
        }
    /* Preserve at least a full octave of musical context on the ruler. */
    lo = ((lo - 200) / 100 - 2) * 100;
    hi = ((hi + 200) / 100 + 2) * 100;
    if (hi - lo < 1200) {
        lo -= (1200 - (hi - lo)) / 2;
        hi = lo + 1200;
    }
    for (j = 0; j < sd.count; j++) {
        span[j] = timing && sd.count > 1 ? sd.gap_s[j] : 1u;
        total += span[j];
    }
    if (!total) total = 1;
    px[0] = x0;
    for (j = 0; j < sd.count; j++)
        px[j + 1] = x0 + (int32_t)(width * (uint32_t)(j + 1) / sd.count);
    if (timing) {
        /* Timing page: every potential node is visible. The final marker
         * represents the next arrival at step 1, never another pitched node. */
        uint32_t acc = 0;
        for (j = 0; j < SD_MAX; j++)
            px[j] = x0 + (int32_t)(width * (uint32_t)j / SD_MAX);
        for (j = 0; j < sd.count && sd.count > 1; j++) {
            acc += span[j];
            px[j + 1] = x0 + (int32_t)(width * acc / total);
        }
        px[sd.count] = x0 + width;
    } else if (sd.count == 1) {
        px[0] = x0;
        px[1] = x0 + width;
    }
    /* Each rung is a note on a common cents scale; accents denote C or A. */
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
    /* Two strands per drone: a subdued backing path plus the vivid melody.
     * On the note page, slightly undulate the connecting path like Setup 2.
     * On the timing page, one clear path per drone, as on Swell. */
    for (g = 1; g >= 0; g--) {
        const uint16_t col = LK_DRONE[g];
        const uint16_t back = lk_mix(0x07060Bu, LK_RAMP[g][0], 0.50f);
        for (j = 0; j < sd.count; j++) {
            const int a = sd_pitch(g, j), b = sd_pitch(g, (j + 1) % sd.count);
            const int dx = px[j + 1] - px[j];
            int32_t priorx = px[j], priory = sd_y(a, lo, hi);
            int m;
            if (!timing) priory += (int32_t)(1.5f * fm_sinf((float)j * 2.1f + (float)g));
            for (m = 1; m <= (dx > 1 ? dx : 1); m++) {
                const float t = (float)m / (float)(dx > 1 ? dx : 1);
                const float smooth = t * t * (3.0f - 2.0f * t);
                const int n = (int)((float)a + (float)(b - a) * smooth + 0.5f);
                const int32_t x = px[j] + m;
                const int32_t wobble = timing ? 0 : (int32_t)(2.5f * fm_sinf(6.2831853f * t + (float)j * 1.6f + (float)g) * fm_sinf(3.1415926f * t));
                const int32_t y = sd_y(n, lo, hi) + wobble;
                if (!timing) cv_line(priorx, priory - 2, x, y - 2, back);
                cv_line(priorx, priory, x, y, col);
                priorx = x;
                priory = y;
            }
        }
        for (j = 0; j < sd.count; j++) {
            const int32_t x = px[j], y = sd_y(sd_pitch(g, j), lo, hi);
            const int chosen = !timing && j == sd.position && g == (sd_linked() ? 0 : sd.drone);
            cv_round(x - (chosen ? 4 : 3), y - (chosen ? 4 : 3),
                     chosen ? 9 : 7, chosen ? 9 : 7, chosen ? 4 : 3, chosen ? LK_WHITE : col);
            cv_round(x - 1, y - 1, 3, 3, 1, LK_BG);
        }
    }
    /* Running cursor: the two drones traverse the same interval map, but
     * drone II has an independent phase. */
    if (sd.enabled && ui.playing && sd.count > 1) {
        const uint32_t duration = sd_total_s() * 1000u;
        for (g = 1; g >= 0; g--) {
            uint32_t t = (plat_ms() - sd.started_ms) % duration;
            const int n = sd_at(g, plat_ms() - sd.started_ms);
            if (g) t = (t + duration * sd.phase / 100u) % duration;
            const int32_t x = x0 + (int32_t)((uint64_t)t * width / duration);
            const int32_t y = sd_y(n, lo, hi);
            cv_round(x - 3, y - 3, 7, 7, 3, LK_WHITE);
            cv_round(x - 2, y - 2, 5, 5, 2, LK_DRONE[g]);
        }
    }
    if (timing) {
        /* A line of grey placeholders shows available nodes without
         * implying any active interval. The active line ends at LOOP. */
        for (j = sd.count; j < SD_MAX; j++) {
            const int32_t x = x0 + (int32_t)((uint32_t)width * j / SD_MAX);
            cv_round(x - 2, 116, 5, 5, 2, LK_UNLIT);
        }
        if (sd.count > 1) {
            for (j = 1; j < sd.count; j++) {
                itoa_u((uint32_t)j + 1u, txt);
                cv_text(px[j] - 3, 108, &FONT_XS, txt, LK_LABEL);
            }
            cv_rect(x0, 117, width, 1, LK_RULE);
            cv_rect(x0, 117, (int32_t)(width * (sd.enabled ? (plat_ms() - sd.started_ms) % (total * 1000u) : 0) /
                                             (total * 1000u)), 1, LK_GOLD);
            cv_text(203, 108, &FONT_XS, "LOOP", LK_LABEL);
        } else {
            cv_text(45, 108, &FONT_XS, "ADD A STEP", LK_LABEL);
        }
    } else {
        for (j = 0; j < sd.count; j++) {
            itoa_u((uint32_t)j + 1u, txt);
            cv_text(px[j] - 3, 108, &FONT_XS, txt, j == sd.position ? LK_WHITE : LK_LABEL);
        }
    }
    cv_text(5, 0, &FONT_XS, timing ? "TIME" : "PITCH", LK_LABEL);
    if (sd_linked() && !timing)
        cv_text(130, 0, &FONT_XS, "PITCHES LINKED", LK_LABEL);
    if (sd.enabled)
        cv_text(202, 0, &FONT_XS, "ON", LK_GOLD);
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
            cv_rect(cx - 22, 72, 44, 1, LK_RULE);
            cv_round(cx - 22 + (int32_t)((i == 3 ? sd_total_s() : sd_node_s(i + 1)) * 43 / SD_LIMIT_S) - 2, 69, 5, 5, 2, col);
        } else if (active && !timing) {
            const int v = i == 0 ? sd.position : i == 1 ? sd.drone : i == 2 ? sd.count - 1 : sd.phase;
            const int max = i == 0 ? sd.count - 1 : i == 1 ? 1 : i == 2 ? 3 : 100;
            lk_dotrow(cx, 72, max > 0 ? (float)v / (float)max : 0.0f, LK_RAMP[0][0], LK_RAMP[0][1], 0, 0);
        }
    }
    cv_blit(0, LK_VAL_Y);
}
