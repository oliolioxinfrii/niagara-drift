/* SPDX-License-Identifier: GPL-3.0-only */
/* Procedural geological and seismographic figures for the 240 x 124 picture
 * band. All textures are deterministic, bounded, and drawn into the existing
 * RGB565 strip buffer. The DSP and parameter tables are not involved. */

/* Each drone has its own hue hierarchy. The separate red-to-yellow meter ramp
 * represents normalized parameter magnitude, not drone identity. */
static const uint16_t VG_STRATA[2][9] = {
    {RGB(255, 220, 64), RGB(255, 189, 45), RGB(252, 146, 30),
     RGB(230, 99, 29), RGB(202, 66, 30), RGB(159, 43, 29),
     RGB(119, 31, 31), RGB(82, 24, 28), RGB(55, 20, 26)},
    {RGB(255, 117, 83), RGB(251, 82, 65), RGB(231, 54, 47),
     RGB(202, 43, 45), RGB(166, 33, 41), RGB(128, 29, 38),
     RGB(101, 27, 36), RGB(77, 23, 32), RGB(53, 20, 27)}
};
static uint32_t vg_hash(uint32_t x)
{
    x ^= x >> 16; x *= 0x7feb352du;
    x ^= x >> 15; x *= 0x846ca68bu;
    return x ^ (x >> 16);
}
static int vg_abs(int x) { return x < 0 ? -x : x; }
static int vg_min(int x, int y) { return x < y ? x : y; }
static int vg_max(int x, int y) { return x > y ? x : y; }
static uint16_t vg_dim(uint16_t c, float f) { return vl_bright(c, f); }

/* Smooth between signed control points, without sampling a sine wave. */
static int vg_interpolate(const int8_t *p, int n, int u, int w)
{
    const int scaled = u * (n - 1), idx = vg_min(scaled / w, n - 2);
    const int t = scaled - idx * w;
    const int f = t * t * (3 * w - 2 * t) * 100 / (w * w * w);
    return ((int)p[idx] * (100 - f) + (int)p[idx + 1] * f) / 100;
}
static int vg_fold(int u, int band, int drone, int strength)
{
    static const int8_t A[2][10] = {
        {0, -2, -1, -5, -9, -12, -3, 5, 3, 1},
        {2, 1, -4, -7, -1, 6, 11, 9, 0, -2}
    };
    const int t = vg_interpolate(A[drone], 10, u, 104);
    const int fault = drone ? 67 : 58;
    const int distance = vg_abs(u - fault);
    const int roll = distance < 29 ? (29 - distance) * strength / 12 : 0;
    const int slip = u > fault + 3 ? (band + 1) * strength / 8 : 0;
    return t * (strength + 4) / 9 + (drone ? -roll : roll) + slip;
}

/* Fine strata are spaced rather than filled, so they survive LCD downscaling
 * without becoming a solid orange block. The sampled stipple falls between
 * adjacent contours; values affect both buckling and apparent density. */
static void vg_section(int drone, int x0, int top, int w, int n, int buckle, float brightness, int detail)
{
    int row, k;
    for (row = n - 1; row >= 0; row--) {
        const uint16_t col = vg_dim(VG_STRATA[drone][row], brightness);
        int x, lastx = x0, lasty = top + row * 8 + vg_fold(0, row, drone, buckle);
        for (x = 2; x <= w; x += 2) {
            const int y = top + row * 8 + vg_fold(x * 104 / w, row, drone, buckle);
            const int nx = x0 + x;
            cv_line(lastx, lasty, nx, y, col);
            if (row < 2) cv_line(lastx, lasty + 1, nx, y + 1, vg_dim(col, 0.37f));
            lastx = nx; lasty = y;
        }
        for (k = 0; k < 18 + 4 * detail; k++) {
            const uint32_t h = vg_hash((uint32_t)(row * 1499 + k * 97 + drone * 131));
            const int u = 3 + (int)(h % (uint32_t)(w - 5));
            const int y = top + row * 8 + vg_fold(u * 104 / w, row, drone, buckle) + 2 + (int)((h >> 8) % 5u);
            if (y >= 24 && y < LK_PIC_H - 2)
                cv_rect(x0 + u, y, (h & 15u) == 0u ? 2 : 1, 1,
                        vg_dim(col, (h & 3u) == 0u ? 0.74f : 0.32f));
        }
    }
}

/* A hand-drawn irregular volcanic profile is sampled through smooth short
 * segments. All nested layers share it, avoiding unrelated zigzags. */
static int vg_ridge(int u)
{
    static const int8_t Y[13] = {78, 74, 76, 70, 67, 63, 54, 42, 47, 56, 69, 72, 75};
    return vg_interpolate(Y, 13, u, 238);
}
static void vg_ground(int x0, int w, int base, int depth, float brightness)
{
    int layer, i;
    for (layer = depth - 1; layer >= 0; layer--) {
        const uint16_t color = vg_dim(VG_STRATA[0][layer], brightness);
        int px = x0, py = vg_ridge(0) + base + layer * (4 + layer / 2);
        for (i = 2; i <= w; i += 2) {
            const int nx = x0 + i;
            const int u = i * 238 / w;
            const int ny = vg_ridge(u) + base + layer * (4 + layer / 2) + (int)((vg_hash((uint32_t)(u / 9 + layer * 13)) & 3u) == 0u);
            if (ny < 105) cv_line(px, py, nx, ny, color);
            px = nx; py = ny;
        }
    }
    for (i = 0; i < 340; i++) {
        uint32_t h = vg_hash((uint32_t)(i * 911 + 457));
        int u = 3 + (int)(h % 233u);
        int y = vg_ridge(u) + base + 3 + (int)((h >> 9) % 27u);
        int band = vg_min(8, (y - vg_ridge(u) - base) / 5);
        if (y >= 34 && y <= 95)
            cv_rect(x0 + u * w / 238, y, (h & 31u) == 0u ? 2 : 1, 1,
                    vg_dim(VG_STRATA[0][band], 0.38f + 0.30f * brightness));
    }
}

/* Tremor-like arrival packets. The trace is intentionally event-driven rather
 * than a conventional repeating oscilloscope waveform. */
static void vg_seismic(int y0, float activity)
{
    static const uint8_t EVENT[6] = {19, 54, 91, 137, 180, 214};
    const int tick = (int)((plat_ms() / 140u) % 37u);
    int x, lasty = y0, lastx = 9, k;
    cv_rect(9, y0, 223, 1, LK_RULE);
    for (x = 9; x <= 231; x += 20) cv_rect(x, y0 - 3, 1, 7, LK_RULE);
    for (x = 9; x <= 231; x++) {
        const uint32_t h = vg_hash((uint32_t)((x + tick) * 41));
        int env = 1;
        for (k = 0; k < 6; k++) {
            const int d = vg_abs(x - (9 + (int)EVENT[k]));
            const int reach = 5 + k % 3 * 4;
            if (d < reach) env += (reach - d) * (k & 1 ? 2 : 3) / 4;
        }
        int amp = (int)((int)(h % 15u) - 7) * env * (int)(3.0f + activity * 6.0f) / 24;
        int py = y0 - vg_max(-10, vg_min(10, amp));
        const uint16_t ink = (vg_abs(amp) >= 7 && activity > 0.55f) ? LK_WHITE :
                             x % 3 == 0 ? VG_STRATA[0][0] : VG_STRATA[0][2];
        cv_line(lastx, lasty, x, py, ink);
        lastx = x; lasty = py;
    }
}
static void vl_surface(int pitch)
{
    int g;
    if (!pitch) {
        const float v = 0.5f * (vl_norm(DR_GP(0, GP_LEVEL)) + vl_norm(DR_GP(1, GP_LEVEL)));
        const float energy = fm_clampf(v * dr_fade, 0.0f, 1.0f);
        cv_text(9, 1, &FONT_XS, "SURFACE / OBSERVATION", LK_LABEL);
        vg_ground(0, 239, -16, 8, 0.67f + 0.33f * energy);
        /* Exposed conduit, incandescent vent and thin drifting plume. */
        cv_line(115, 38, 120, 71, VG_STRATA[0][0]);
        cv_line(118, 40, 117, 70, VG_STRATA[0][3]);
        for (g = 0; g < 5; g++) {
            int i, ox = 115 + g * 2, oy = 39;
            for (i = 1; i <= 9; i++) {
                const int nx = 115 + g * 2 + i * i / 10 + (i * (g + 1)) % 5 - 2;
                const int ny = 39 - 4 * i;
                cv_line(ox, oy, nx, ny, g & 1 ? VG_STRATA[1][1] : VG_STRATA[0][3]);
                ox = nx; oy = ny;
            }
        }
        for (g = 0; g < 43; g++) {
            uint32_t h = vg_hash((uint32_t)(g * 67 + 23));
            int x = 109 + (int)(h % 52u);
            int y = (int)((h >> 7) % 47u);
            cv_rect(x, y, 1, 1, g % 7 == 0 ? LK_WHITE : VL_ROCK);
        }
        cv_rect(8, 93, 224, 1, LK_RULE);
        vg_seismic(108, energy);
        return;
    }
    /* The second HOME page exposes the two crusts independently. */
    cv_rect(119, 3, 1, 117, LK_RULE);
    for (g = 0; g < 2; g++) {
        const int x0 = 8 + 120 * g;
        const float res = vl_norm(DR_GP(g, GP_RESO));
        const float level = vl_norm(DR_GP(g, GP_LEVEL));
        const float pitchpos = vl_norm(DR_GP(g, GP_PITCH));
        const int fold = 2 + (int)(res * 5.0f + pitchpos * 2.0f);
        char note[16];
        cv_text(x0 + 2, 2, &LK_F_DRONE, g ? "II" : "I", LK_DRONE[g]);
        vg_section(g, x0 + 1, 34, 103, 9, fold, 0.55f + 0.45f * level, 1);
        dr_param_text(DR_GP(g, GP_PITCH), gpar(g, GP_PITCH), note);
        cv_text(x0 + 3, 104, &FONT_XS, note, LK_WHITE);
        cv_rect(x0 + 1, 119, 103, 1, LK_RULE);
    }
}
static void vl_mantle_sources(int sub)
{
    int g;
    cv_rect(119, 3, 1, 117, LK_RULE);
    for (g = 0; g < 2; g++) {
        const int x0 = 8 + 120 * g;
        const float shape = vl_norm(DR_GP(g, GP_WAVE));
        const float mix = vl_norm(DR_GP(g, GP_MIX));
        const float drive = vl_norm(DR_GP(g, GP_DRIVE));
        const float vol = vl_norm(DR_GP(g, GP_LEVEL));
        const int fold = 3 + (int)(5.0f * (sub ? drive : shape));
        int k;
        cv_text(x0 + 2, 2, &LK_F_DRONE, g ? "II" : "I", LK_DRONE[g]);
        vg_section(g, x0 + 1, 28, 103, 10, fold, 0.68f + vol * 0.28f, 2);
        if (sub && mix > 0.01f) {
            /* A subharmonic intrusion cuts through otherwise continuous beds. */
            const int xx = x0 + 55;
            for (k = 0; k < 16; k++) {
                const int x = xx + (int)(vg_hash((uint32_t)k * 23u) % 7u) - 3;
                const int y = 26 + k * 5;
                cv_line(x, y, x + 1, y + 5,
                        k % 5 == 0 && drive > 0.80f ? LK_WHITE :
                        vg_dim(VG_STRATA[g][0], mix));
            }
        } else if (!sub) {
            /* Fine wave-width ticks provide information without a scope trace. */
            const int count = 3 + (int)(shape * 11.0f);
            for (k = 0; k < count; k++)
                cv_rect(x0 + 8 + k * 7, 116, 3, 2,
                        k == count - 1 ? LK_WHITE : VG_STRATA[g][2]);
        }
    }
}
/* The same irregular volcano section is used by LIFE, but the four parameter
 * families affect different physical marks: bulk plume, deflection, ejecta,
 * and fine ash/weathering. */
static void vg_life_cone(float age, float character)
{
    static const int16_t X[13] = {5, 24, 42, 62, 77, 94, 106, 117, 129, 153, 177, 204, 234};
    static const int16_t Y[13] = {118, 113, 110, 100, 88, 78, 70, 71, 79, 93, 105, 113, 119};
    int band, i, k;
    for (band = 6; band >= 0; band--) {
        const uint16_t c = vg_dim(VG_STRATA[0][vg_min(8, band + 1)], 0.80f - age * 0.15f);
        for (i = 1; i < 13; i++) {
            const int a = vg_min(121, Y[i - 1] + band * (3 + band / 3));
            const int b = vg_min(121, Y[i] + band * (3 + band / 3));
            cv_line(X[i-1], a, X[i], b, c);
        }
    }
    for (k = 0; k < 230; k++) {
        const uint32_t h = vg_hash((uint32_t)(k * 191 + 59));
        const int x = 7 + (int)(h % 225u);
        const int y = 75 + (int)((h >> 8) % 46u);
        if (y >= 82 + vg_abs(x - 111) / 4 || y >= 111)
            cv_rect(x, y, 1, 1, vg_dim(VG_STRATA[0][3 + k % 5], 0.25f + age * 0.24f));
    }
    /* A vent can grow incandescent without assigning red to "overdrive." */
    for (k = 0; k < (int)(character * 23.0f); k++) {
        uint32_t h = vg_hash((uint32_t)(k * 71 + 311));
        int x = 109 + (int)(h % 17u) - 8;
        int y = 73 - (int)((h >> 8) % 28u);
        cv_rect(x, y, 1 + (k % 11 == 0), 1, k % 3 == 0 ? LK_WHITE : VL_SURFACE);
    }
}
static void vl_life(void)
{
    const float feedback = vl_norm(P_FEEDBACK), drift = vl_norm(P_DRIFT);
    const float character = vl_norm(P_CHAR), age = vl_norm(P_AGE);
    const float phase = (float)(plat_ms() % 5000u) * 0.00032f;
    const int strands = 3 + (int)(feedback * 11.0f);
    int k, i;
    cv_text(9, 1, &FONT_XS, "VENT / PLUME ACTIVITY", LK_LABEL);
    vg_life_cone(age, character);
    for (k = strands - 1; k >= 0; k--) {
        const float offset = (float)k - 0.5f * (float)(strands - 1);
        int px = 113 + (int)offset, py = 72;
        const uint16_t col = k == 0 && feedback > 0.76f ? LK_WHITE :
                             vg_dim(VG_STRATA[0][k % 5], 0.44f + feedback * 0.54f);
        for (i = 1; i <= 30; i++) {
            const float t = (float)i / 30.0f;
            const float wave = fm_sinf(0.32f * (float)i + phase + 0.38f * (float)k);
            const int x = 113 + (int)(drift * 41.0f * t * t +
                                      wave * (2.0f + feedback * 4.0f) +
                                      offset * (1.0f + feedback * t * 1.6f));
            const int y = 72 - i * 2 - i / 4;
            cv_line(px, py, x, y, col);
            px = x; py = y;
        }
    }
    for (k = 0; k < 90 + (int)(age * 125.0f) + (int)(feedback * 55.0f); k++) {
        const uint32_t h = vg_hash((uint32_t)(k * 83 + 621));
        const float t = (float)(h % 100u) * 0.01f;
        const int yc = 69 - (int)(t * 66.0f);
        const int xc = 113 + (int)(drift * 41.0f * t * t +
                           fm_sinf(5.6f * t + phase) * 4.0f);
        const int width = 4 + (int)((feedback * 15.0f + age * 10.0f) * t);
        const int x = xc + (int)((h >> 9) % (uint32_t)(2 * width + 1)) - width;
        const int y = yc + (int)((h >> 16) % 11u) - 5;
        if (y >= 3 && y < 70)
            cv_rect(x, y, 1, 1,
                    k % 11 == 0 && feedback > 0.8f ? LK_WHITE :
                    vg_dim(k & 1 ? VL_MANTLE : LK_LABEL, 0.24f + age * 0.55f));
    }
}
/* Noise is a frequency-distributed particulate field, not a cone outline.
 * Pink weights low frequencies; white distributes energy more evenly. */
static void vl_tone(void)
{
    int g;
    cv_rect(119, 3, 1, 117, LK_RULE);
    for (g = 0; g < 2; g++) {
        const int x0 = 9 + 120 * g, noise = gpar(g, GP_NOISE);
        const float level = vl_norm(DR_GP(g, GP_NOISE));
        const float pink = vl_norm(DR_GP(g, GP_NCOLOR));
        const uint16_t col = LK_DRONE[g];
        const int count = (int)(level * 320.0f);
        int x, j;
        cv_text(x0, 2, &FONT_XS, g ? "ASH / II" : "ASH / I", col);
        for (j = 0; j < 4; j++) {
            int px = x0 + 2, py = 102;
            for (x = 0; x <= 100; x += 2) {
                const float fraction = (float)x * 0.01f;
                const float slope = (1.0f - pink) * 0.88f + pink * (1.0f - 0.68f * fraction);
                const int height = (int)(level * slope * (66.0f - 5.0f * (float)j));
                const int y = 100 - height + j * 3 + (int)(vg_hash((uint32_t)(x / 7 + j * 17)) % 3u) - 1;
                cv_line(px, py, x0 + x + 2, y, vg_dim(col, 0.9f - 0.18f * (float)j));
                px = x0 + x + 2; py = y;
            }
        }
        for (j = 0; j < count; j++) {
            const uint32_t h = vg_hash((uint32_t)(j * 181 + g * 901 + 39));
            const int x = (int)(h % 102u);
            const float fraction = (float)x / 102.0f;
            const float spectrum = (1.0f - pink) * 0.88f + pink * (1.0f - 0.68f * fraction);
            const int edge = 100 - (int)(level * 69.0f * spectrum);
            const int y = edge + (int)((h >> 10) % (uint32_t)vg_max(1, 100 - edge));
            const uint16_t dust = j % 13 == 0 ? LK_WHITE :
                                  vg_dim(VG_STRATA[g][j % 6], 0.55f);
            cv_rect(x0 + x, y, j % 29 == 0 ? 2 : 1, 1, dust);
        }
        cv_rect(x0 + 1, 108, 103, 1, LK_RULE);
        cv_text(x0 + 2, 109, &FONT_XS, "LOW", LK_LABEL);
        cv_text(x0 + 69, 109, &FONT_XS, "HIGH", LK_LABEL);
        (void)noise;
    }
}
/* Seismographic propagation: discrete delay arrivals and a dissipating
 * reverberant coda, displayed on separate horizontal channels. */
static void vl_air(void)
{
    const float repeats = vl_norm(P_DLY_REPEATS), wet_delay = vl_norm(P_DLY_MIX);
    const float wet_reverb = vl_norm(P_REV_MIX), decay = vl_norm(P_REV_LEN);
    const float damping = vl_norm(P_REV_DAMP);
    const float drive = vl_norm(P_DLY_DRIVE);
    const int period = 19 + (int)(vl_norm(P_DLY_TIME) * 34.0f);
    const int hold = proj.par[P_REV_LEN] >= 100;
    int k, x, p;
    cv_text(9, 1, &FONT_XS, "DELAY / ARRIVALS", LK_LABEL);
    cv_text(9, 65, &FONT_XS, hold ? "REVERB / HELD FIELD" : "REVERB / CODA", LK_LABEL);
    for (k = 0; k < 2; k++) {
        const int center = k ? 94 : 36;
        const int top = k ? 76 : 17;
        const int bottom = k ? 113 : 57;
        cv_rect(8, center, 224, 1, LK_RULE);
        for (x = 10; x <= 230; x += 22)
            cv_rect(x, top, 1, bottom - top, LK_RULE);
        for (p = 0; p < 3; p++) {
            int px = 10, py = center + p * 2 - 2;
            for (x = 10; x <= 230; x++) {
                const uint32_t h = vg_hash((uint32_t)((x + p * 11 + (plat_ms() / 210u) % 23u) * 89));
                const int noise = (int)(h % 19u) - 9;
                float env = 0.0f;
                if (!k) {
                    const int pos = x - 18;
                    if (pos >= 0) {
                        const int arrival = pos % period, nth = pos / period;
                        const float repeat_gain = 1.0f - (1.0f - repeats) * fm_clampf((float)nth * 0.13f, 0.0f, 1.0f);
                        if (arrival < 15)
                            env = wet_delay * (1.0f - (float)arrival / 15.0f) * repeat_gain;
                    }
                } else {
                    const int tail = 22 + (int)(decay * 180.0f);
                    const int t = x - 21;
                    if (t >= 0 && (hold || t < tail))
                        env = wet_reverb * (hold ? 0.9f : (1.0f - (float)t / (float)tail)) *
                              (0.75f + 0.25f * (1.0f - damping));
                }
                {
                    const int a = (int)((float)noise * env * (k ? 1.3f : 1.65f));
                    const int yy = center + p * 2 - 2 + vg_max(-14, vg_min(14, a));
                    const uint16_t ink = (vg_abs(a) >= 10 && (drive > 0.72f || hold)) ?
                                         LK_WHITE : vg_dim(VL_AIR, p == 2 ? 1.0f : 0.36f + 0.21f * (float)p);
                    cv_line(px, py, x, yy, ink);
                    px = x; py = yy;
                }
            }
        }
        if (!k) {
            for (x = 18; x <= 229; x += period) {
                cv_rect(x, 21, 1, 4, VL_AIR);
                cv_rect(x, 58, 1, 2, LK_LABEL);
            }
        } else {
            cv_rect(8, 118, 224, 1, LK_RULE);
            cv_text(10, 119, &FONT_XS, hold ? "FIELD LOCKED" : "PROPAGATION", LK_LABEL);
        }
    }
}
/* A cutaway globe: muted observational meridians on the left, fiery strata
 * on the right. It is an emblem; all controls retain the common value band. */
static void vl_planet(void)
{
    const int cx = 63, cy = 65, r = 48;
    static const uint16_t colors[5] = {RGB(248, 203, 58), RGB(242, 144, 41),
                                       RGB(232, 96, 43), RGB(190, 53, 44),
                                       RGB(125, 35, 37)};
    int i, k;
    lk_circle(cx, cy, r, LK_WHITE);
    for (k = 0; k < 5; k++) {
        const int rr = r - 7 - 8 * k;
        int px = cx, py = cy - rr;
        for (i = 1; i <= 32; i++) {
            const float a = 3.14159265f * (float)i / 32.0f;
            const int x = cx + (int)((float)rr * fm_sinf(a));
            const int y = cy - (int)((float)rr * fm_sinf(a + 1.5707963f));
            cv_line(px, py, x, y, colors[k]);
            px = x; py = y;
        }
    }
    /* Sparse longitude/latitude clues; no continents or illustrative land. */
    for (i = -3; i <= 3; i++) {
        int y = cy + i * 11;
        int dx = (int)((float)r * fm_sinf(1.5707963f * (1.0f - (float)(i * i) / 22.0f)));
        cv_line(cx - dx, y, cx - 3, y, vg_dim(LK_LABEL, 0.34f));
    }
    for (k = 0; k < 2; k++) {
        const int rx = 17 + k * 13;
        int px = cx, py = cy - r;
        for (i = 1; i <= 24; i++) {
            const float a = 3.14159265f * (float)i / 24.0f;
            const int x = cx - (int)((float)rx * fm_sinf(a));
            const int y = cy - (int)((float)r * fm_sinf(a + 1.5707963f));
            cv_line(px, py, x, y, vg_dim(LK_WHITE, 0.32f));
            px = x; py = y;
        }
    }
    cv_rect(cx, 16, 1, 99, LK_WHITE);
    for (i = 0; i < 16; i++) {
        float a = 6.2831853f * (float)i / 16.0f;
        int x0 = cx + (int)(50.0f * fm_sinf(a));
        int y0 = cy + (int)(50.0f * fm_sinf(a + 1.5707963f));
        int x1 = cx + (int)(54.0f * fm_sinf(a));
        int y1 = cy + (int)(54.0f * fm_sinf(a + 1.5707963f));
        cv_line(x0, y0, x1, y1, LK_LABEL);
    }
    cv_text(129, 13, &FONT_T20, "PLANET", LK_WHITE);
    cv_rect(128, 39, 102, 2, VL_SURFACE);
    cv_text(130, 55, &LK_F_SMALL,
            ui.view == V_SETUP ? "CLOCK / TIME" :
            ui.view == V_SETUP2 ? "TUNING" : "SPEAKER", LK_WHITE);
    cv_text(130, 85, &FONT_XS,
            ui.view == V_SETUP ? "SYSTEM TIMING" :
            ui.view == V_SETUP2 ? "REFERENCE" : "OUTPUT", LK_LABEL);
}
