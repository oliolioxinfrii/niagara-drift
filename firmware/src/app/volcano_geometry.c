/* SPDX-License-Identifier: GPL-3.0-only */
/* Procedural geological and seismographic figures for the 240 x 124 picture
 * band. All textures are deterministic, bounded, and drawn into the existing
 * RGB565 strip buffer. Live parameters are read, not modified. */

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

/* A low-bandwidth observatory trace derived from live scope samples.
 * A nine-tap symmetric low-pass reconstruction suppresses oscilloscope
 * aliasing; the faint outer curve reports the current filter response.
 * Neither trace fabricates periodic seismic packets. */
static void vg_seismic(int drone, int x0, int y0, float level)
{
    const int width = 104;
    const uint16_t color = LK_DRONE[drone];
    const float loud = fm_clampf(level, 0.0f, 1.0f);
    float peak = 0.015f;
    int j, i;
    int prev_wave_y = y0, prev_env_y = y0, prev_x = x0;
    for (i = 0; i < DR_SCOPE_N; i++)
        peak = fm_maxf(peak, fm_fabsf(dr_scope[drone][i]));
    cv_rect(x0, y0, width, 1, LK_RULE);
    for (j = 0; j <= width; j += 12)
        cv_rect(x0 + j, y0 - 3, 1, 7, LK_RULE);
    for (j = 0; j < width; j += 2) {
        const int ix = j * (DR_SCOPE_N - 1) / (width - 1);
        const float hz = 100.0f * (float)j / (float)(width - 1);
        const float db = filt_db(drone, dr_cut[drone], hz);
        const float response = fm_clampf((db + 36.0f) / 48.0f, 0.0f, 1.0f);
        float weighted = 0.0f;
        int tap, weights = 0;
        for (tap = -4; tap <= 4; tap++) {
            const int pos = vg_max(0, vg_min(DR_SCOPE_N - 1, ix + 2 * tap));
            const int weight = 5 - vg_abs(tap);
            weighted += dr_scope[drone][pos] * (float)weight;
            weights += weight;
        }
        const float sample = weighted / ((float)weights * peak);
        const float envelope = 10.0f * loud * response;
        const int x = x0 + j;
        const int ey = y0 - (int)(envelope + 0.5f);
        const int sy = y0 - (int)(envelope * sample + (sample >= 0.0f ? 0.5f : -0.5f));
        if (j) {
            cv_line(prev_x, prev_env_y, x, ey, vl_bright(color, 0.48f));
            cv_line(prev_x, prev_wave_y, x, sy,
                    fm_fabsf(sample) > 0.9f && loud > 0.75f ? LK_WHITE : color);
        }
        prev_x = x; prev_env_y = ey; prev_wave_y = sy;
    }
}
static void vg_lava_flow(float level)
{
    /* At zero level there is no lava. At full level the front has risen
     * through the conduit, crossed the vent and run down the left flank. */
    static const int16_t x[] = {119, 119, 119, 119, 119, 118, 118, 118, 117, 117,
                                118, 118, 111, 101, 89, 74, 59, 41, 21, 1};
    const int n = (int)(sizeof x / sizeof x[0]);
    int16_t y[sizeof x / sizeof x[0]];
    const float advance = level * (float)(n - 1);
    int j;
    if (level <= 0.0f) return;
    y[0] = 122; y[1] = 112; y[2] = 103; y[3] = 94; y[4] = 85;
    y[5] = 76; y[6] = 67; y[7] = 58; y[8] = 49; y[9] = 40;
    y[10] = 32; y[11] = (int16_t)(vg_ridge(118) - 18);
    for (j = 12; j < n; j++) y[j] = (int16_t)(vg_ridge(x[j]) - 17);
    for (j = 1; j < n; j++) {
        const float remaining = advance - (float)(j - 1);
        int xe, ye;
        if (remaining <= 0.0f) break;
        if (remaining >= 1.0f) {
            xe = x[j]; ye = y[j];
        } else {
            xe = x[j - 1] + (int)((float)(x[j] - x[j - 1]) * remaining);
            ye = y[j - 1] + (int)((float)(y[j] - y[j - 1]) * remaining);
        }
        cv_line(x[j - 1] - 1, y[j - 1], xe - 1, ye, VL_MANTLE);
        cv_line(x[j - 1] + 1, y[j - 1], xe + 1, ye, VL_CRUST);
        cv_line(x[j - 1], y[j - 1], xe, ye, VL_SURFACE);
        if (remaining < 1.0f) break;
    }
    /* Small incandescent front moves only within the exposed flow. */
    if (advance > 2.0f) {
        const int k = 2 + (int)((plat_ms() / 140u) % (uint32_t)vg_max(1, (int)advance - 1));
        if (k < n) cv_rect(x[k] - 1, y[k] - 1, 3, 3, LK_WHITE);
    }
}
static void vg_smoke(float level)
{
    const int strands = (int)(10.0f * level + 0.5f);
    const float drift = vl_norm(P_DRIFT);
    const float time = (float)(plat_ms() % 4500u) * 0.0014f;
    int k, i;
    if (!strands) return;
    for (k = 0; k < strands; k++) {
        int px = 117 + k / 2, py = vg_ridge(118) - 20;
        const float spread = (float)(k - strands / 2);
        for (i = 1; i <= 22; i++) {
            const float t = (float)i / 22.0f;
            const int nx = 117 + (int)(spread * (1.0f + t * 1.2f) + 25.0f * drift * t * t +
                                      4.0f * fm_sinf((float)i * 0.4f + (float)k * 0.34f + time));
            const int ny = vg_ridge(118) - 20 - i * 2;
            cv_line(px, py, nx, ny,
                    vg_dim(k % 4 == 0 ? VL_CRUST : k % 3 == 0 ? LK_LABEL :
                           VG_STRATA[1][2 + k % 5], 0.43f + 0.56f * level));
            px = nx; py = ny;
        }
    }
    for (i = 0; i < (int)(level * 80.0f); i++) {
        const uint32_t h = vg_hash((uint32_t)(i * 149 + (plat_ms() / 320u) % 13u));
        const int yy = (int)((h >> 9) % 47u);
        const int xx = 116 + (int)(h % 57u) - 13 + (46 - yy) / 7;
        if (xx > 0 && xx < 239)
            cv_rect(xx, yy, 1, 1, vg_dim(i % 9 == 0 ? VL_SURFACE : VG_STRATA[1][2 + i % 5],
                                        0.50f + 0.30f * level));
    }
}
static void vl_surface(int pitch)
{
    int g;
    if (!pitch) {
        const float lava = vl_norm(DR_GP(0, GP_LEVEL)) * dr_fade;
        const float smoke = vl_norm(DR_GP(1, GP_LEVEL)) * dr_fade;
        cv_text(9, 1, &FONT_XS, "SURFACE / OBSERVATION", LK_LABEL);
        vg_ground(0, 239, -16, 8, 0.52f); /* Geological beds do not change brightness. */
        vg_lava_flow(lava);
        vg_smoke(smoke);
        cv_rect(119, 96, 1, 27, LK_RULE);
        for (g = 0; g < 2; g++) {
            const int x0 = g ? 127 : 9;
            const float level = vl_norm(DR_GP(g, GP_LEVEL)) * dr_fade;
            cv_text(x0 + 2, 93, &FONT_XS, g ? "II" : "I", LK_DRONE[g]);
            vg_seismic(g, x0, 112, level);
        }
        return;
    }
    /* Pitch retains the dual buckling crust diagrams. */
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
/* Character is vent energy, not just sporadic ejecta: the lava rises up
 * a separate conduit then progressively spills over the left flank. */
static void vg_life_lava(float character)
{
    /* The magma enters at the bottom-most visible scanline before
     * ascending through the full cone and spilling toward the left. */
    static const int16_t x[] = {111, 111, 111, 111, 111, 111, 111, 110, 108, 105,
                                100, 94, 86, 78, 67, 55, 42, 25, 5};
    static const int16_t y[] = {123, 119, 115, 110, 103, 95, 85, 76, 70, 68,
                                72, 76, 80, 85, 96, 103, 110, 115, 118};
    const int segments = (int)(sizeof x / sizeof x[0]) - 1;
    const float advance = fm_clampf(character, 0.0f, 1.0f) * (float)segments;
    int j;
    if (advance <= 0.0f) return;
    /* A short magma reservoir at the base makes the origin visible. */
    cv_line(103, 122, 120, 122, vl_bright(VL_MANTLE, 0.45f + 0.55f * character));
    cv_rect(109, 121, 4, 2, VL_SURFACE);
    for (j = 1; j <= segments; j++) {
        const float partial = advance - (float)(j - 1);
        int xe, ye;
        if (partial <= 0.0f) break;
        if (partial >= 1.0f) {
            xe = x[j]; ye = y[j];
        } else {
            xe = x[j - 1] + (int)((float)(x[j] - x[j - 1]) * partial);
            ye = y[j - 1] + (int)((float)(y[j] - y[j - 1]) * partial);
        }
        cv_line(x[j - 1] - 1, y[j - 1], xe - 1, ye, VL_MANTLE);
        cv_line(x[j - 1] + 1, y[j - 1], xe + 1, ye, VL_CRUST);
        cv_line(x[j - 1], y[j - 1], xe, ye, VL_SURFACE);
        if (partial < 1.0f) break;
    }
    if (advance > 2.0f) {
        const int k = 2 + (int)((plat_ms() / 160u) % (uint32_t)vg_max(1, (int)advance - 1));
        if (k <= segments) cv_rect(x[k] - 1, y[k] - 1, 3, 3, LK_WHITE);
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
    vg_life_lava(character);
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
        const int x0 = 9 + 120 * g;
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
    }
}
/* Seismographic propagation: discrete delay arrivals and a dissipating
 * reverberant coda, displayed on separate horizontal channels. */
/* Delay is displayed as individual arrivals; reverb is the much longer
 * decaying coda. A perceptual amplitude mapping makes quiet mixes visible.
 * The visual noise texture scrolls forward then backward along the same
 * 22-pixel path. Cosine easing makes both turnarounds continuous (no seam or
 * abrupt reversal); interpolation between adjacent noise samples keeps the
 * waveform moving smoothly instead of changing in 210 ms steps. */
#define VL_AIR_LOOP_MS (44u * 210u)
static float vl_air_scroll(uint32_t now_ms)
{
    const float phase = (float)(now_ms % VL_AIR_LOOP_MS) *
                        (6.2831853f / (float)VL_AIR_LOOP_MS);
    return 11.0f * (1.0f - fm_cosf(phase)); /* 0 -> 22 -> 0 */
}
/* Smooth deterministic band-limited texture: an illustrated spectral layer,
 * not FFT data. The three spacings read as body, diffusion and treble. */
static float vg_air_band(float position, int spacing, uint32_t seed)
{
    const float u = position / (float)spacing;
    const uint32_t cell = (uint32_t)u;
    float t = u - (float)cell;
    const uint32_t a = vg_hash(cell * 149u + seed);
    const uint32_t b = vg_hash((cell + 1u) * 149u + seed);
    t = t * t * (3.0f - 2.0f * t);
    return ((float)(a & 2047u) * (1.0f - t) + (float)(b & 2047u) * t - 1023.5f)
            * (1.0f / 1023.5f);
}
/* Follow Shimmer's 8-second wash in and 6-second wash out. This is strictly
 * visual state; the DSP has its own audio ramp and no state is copied back. */
static float vg_air_halo(uint32_t now, int type)
{
    static uint32_t last;
    static float amount;
    const float elapsed = last ? fm_minf((float)(now - last), 30000.0f) : 0.0f;
    last = now;
    amount = type == 4 ? fm_minf(1.0f, amount + elapsed / 8000.0f)
                       : fm_maxf(0.0f, amount - elapsed / 6000.0f);
    return amount;
}
/* Visual descriptors mirror RV_TYPE in drone_fx.c: pre-delay (ms), network
 * size (percent), delay-line modulation (half-rate samples), input lowcut
 * (Hz), and width. Time is deliberately not a linear seconds-per-pixel plot:
 * the left 60px resolve predelay and diffusion, the rest resolve decay. */
static void vg_air_reverb(float wet, float length, float damp, int hold,
                          float scroll, uint32_t now)
{
    static const struct {
        uint8_t predelay, size, mod, lowcut, width;
    } type_map[5] = {
        {4, 35, 2, 120, 80},       /* Room */
        {18, 75, 5, 90, 90},       /* Hall */
        {42, 100, 7, 60, 85},     /* Cathedral */
        {24, 85, 18, 90, 145},    /* Chorus Hall */
        {18, 75, 5, 90, 90}        /* Shimmer: Hall plus octave-up input */
    };
    static const char *const name[5] = {"ROOM", "HALL", "CATH", "CHORUS", "SHIM"};
    const int v = proj.par[P_REV_TYPE];
    const int type = (unsigned)v < 5u ? v : 2;
    /* Fourth-root response preserves continuous zero, but keeps the
     * structure visible at 15-50% wet instead of only near full mix. */
    const float gain = fm_sqrtf(fm_sqrtf(wet));
    const float halo = vg_air_halo(now, type);
    const float drive = proj.par[P_DLY_ROUTE] == 0 ? vl_norm(P_DLY_DRIVE) : 0.0f;
    const float start = 13.0f + 0.85f * (float)type_map[type].predelay;
    const float spread = 5.0f + 0.18f * (float)type_map[type].size;
    const float tail_px = 24.0f + 177.0f * length;
    const float body_gain = 0.80f + (120.0f - (float)type_map[type].lowcut) / 300.0f;
    const float swing = 0.39f * (float)type_map[type].mod;
    const float phase = (float)(now % VL_AIR_LOOP_MS) *
                        (6.2831853f / (float)VL_AIR_LOOP_MS);
    const uint16_t col[4] = {
        lk_mix(LK_FX_RAMP[0], LK_FX_RAMP[1], 0.10f),
        lk_mix(LK_FX_RAMP[0], LK_FX_RAMP[1], 0.53f),
        lk_mix(LK_FX_RAMP[0], LK_FX_RAMP[1], 0.90f),
        lk_mix(LK_FX_RAMP[0], LK_FX_RAMP[1], 1.00f)
    };
    const int nlayer = halo > 0.015f ? 4 : 3;
    int prev[4] = {94, 94, 94, 94}, prev_echo = 94;
    int x, layer;
    text_c(205, 64, &FONT_XS, name[type], VL_AIR);
    cv_rect(70, 78, 1, 34, vg_dim(VL_AIR, 0.19f));
    cv_rect((int)start, 78, 1, 4, vg_dim(VL_AIR, 0.66f));
    for (x = 10; x <= 230; x++) {
        float attack = fm_clampf(((float)x - start) / spread, 0.0f, 1.0f);
        const float tail_t = fm_clampf(((float)x - 70.0f) / tail_px, 0.0f, 1.0f);
        const float tail = 1.0f - tail_t;
        const float age = fm_clampf(((float)x - start) / 170.0f, 0.0f, 1.0f);
        /* DAMP is a low-pass within the feedback lines: body remains,
         * treble vanishes, and the middle band loses its small wiggles.
         * At Infinite LENGTH the DSP explicitly bypasses damping. */
        const float detail = hold ? 1.0f :
            (1.0f - damp) * (1.0f - damp) * (1.0f - 0.55f * damp * age);
        const float mid_detail = hold ? 1.0f : 1.0f - 0.45f * damp * age;
        const float wobble = swing * fm_sinf(0.075f * (float)x + phase);
        const float early = (float)x < start + 65.0f ?
            fm_sinf(((float)x - start) * (type == 0 ? 0.47f : 0.23f) + phase * 0.2f) :
            0.0f;
        const float pos = (float)x + scroll;
        const float notes[4] = {
            vg_air_band(pos, 14, 109u) * 9.0f * body_gain,
            (vg_air_band(pos + 3.0f, 5, 571u) * 12.0f +
                early * (type == 0 ? 4.0f : 2.3f)) * mid_detail,
            vg_air_band(pos + 7.0f, 2, 1079u) * 12.0f * detail,
            vg_air_band((float)x * 1.8f + scroll * 1.25f, 2, 2221u) *
                10.0f * halo * detail
        };
        float env;
        attack = attack * attack * (3.0f - 2.0f * attack);
        env = gain * attack * (hold ? 1.0f : tail * (0.42f + 0.58f * tail));
        for (layer = 0; layer < nlayer; layer++) {
            const float bend = layer == 0 ? 0.35f : layer == 1 ? 1.0f :
                               layer == 2 ? -0.70f : 0.45f;
            const int y = vg_max(78, vg_min(110, 94 +
                          (int)(env * (notes[layer] + bend * wobble))));
            if (x > 10 && gain > 0.0f) {
                uint16_t ink = layer == 2 ? vg_dim(col[layer], 0.12f + 0.88f * detail) :
                               layer == 3 ? vg_dim(col[layer], 0.30f + 0.70f * detail) :
                               col[layer];
                /* Drive precedes reverb only on the combined/Both route.
                 * Other sends bypass the drive and stay in the blue palette. */
                if (layer == 1 && drive > 0.0f)
                    ink = lk_mix(0x70B8DAu, 0xE86231u, 0.48f * drive);
                cv_line(x - 1, prev[layer], x, y, ink);
            }
            prev[layer] = y;
        }
        /* Chorus Hall's 1.45-wide, heavily modulated delay lines form a
         * separate, dimmer strand rather than merely higher amplitude. */
        if (type == 3) {
            const float width = (float)type_map[type].width / 100.0f;
            const int y = vg_max(78, vg_min(110, 94 +
                          (int)(env * (-0.62f * notes[1] - width * wobble))));
            if (x > 10 && gain > 0.0f)
                cv_line(x - 1, prev_echo, x, y, vg_dim(col[1], 0.51f));
            prev_echo = y;
        }
    }
}
/* These profiles mirror the delay's tonal/stereo/modulation distinctions
 * in drone_fx.c. They are deterministic drawings, never sampled audio. */
static void vg_air_delay(float wet, float repeats, float drive, int period,
                         float scroll, uint32_t now)
{
    static const struct {
        float spread, tone, wow, ping, grain, self;
    } type_map[6] = {
        {1.00f, 0.86f,  0.0f, 0.0f,  0.0f, 0.00f}, /* Clean */
        {0.75f, 0.57f, 14.0f, 0.0f,  0.0f, 0.00f}, /* Wide */
        {1.00f, 0.50f, 14.0f, 1.0f,  0.0f, 0.00f}, /* Ping-pong */
        {1.00f, 0.39f, 30.0f, 0.0f,  0.0f, 0.00f}, /* Tape */
        {1.00f, 0.39f, 30.0f, 0.0f, 60.0f, 0.00f}, /* Grain */
        {1.00f, 0.39f, 30.0f, 0.0f,  0.0f, 0.25f}  /* Self */
    };
    static const char *const labels[6] = {"CLEAN", "WIDE", "PING", "TAPE", "GRAIN", "SELF"};
    const int v = proj.par[P_DLY_TYPE];
    const int type = (unsigned)v < 6u ? v : 3;
    const float gain = fm_sqrtf(fm_sqrtf(wet)); /* readable without 100% wet */
    const float phase = (float)(now % VL_AIR_LOOP_MS) * (6.2831853f / (float)VL_AIR_LOOP_MS);
    const float flutter = type_map[type].wow / 30.0f;
    const float warm = fm_clampf(drive * 1.25f, 0.0f, 1.0f);
    const uint16_t driven_ink = lk_mix(0x83D8DBu, 0xEB6333u, warm);
    const int scroll_step = (int)scroll;
    const float scroll_frac = scroll - (float)scroll_step;
    int p, x, n;
    text_c(204, 1, &FONT_XS, labels[type], VL_AIR);
    /* DRIVE processes the dry signal + echoes AFTER the delay. Even if
     * Delay MIX is zero, this orange indicator remains visible. */
    if (drive > 0.005f)
        cv_rect(172, 13, (int)(55.0f * drive + 0.5f), 2, driven_ink);
    cv_rect(8, 37, 224, 1, LK_RULE);
    for (x = 10; x <= 230; x += 22)
        cv_rect(x, 17, 1, 40, LK_RULE);
    for (p = 0; p < 3; p++) {
        int px = 10, py = 37 + p * 2 - 2;
        for (x = 10; x <= 230; x++) {
            const int pos = x - 18;
            int center = 37 + p * 2 - 2;
            float env = 0.0f, noise = 0.0f;
            if (pos >= 0) {
                /* Wide's right line precedes the left by its real .75
                 * delay ratio; Ping alternates a left and right arrival.
                 * Tape, Grain and Self deform the return-time slightly. */
                float warped = (float)pos;
                if (type == 1 && p == 2)
                    warped += (1.0f - type_map[type].spread) * (float)period;
                if (type >= 3)
                    warped += flutter * 1.7f * fm_sinf(0.054f * (float)x + phase);
                if (type == 4)
                    warped += type_map[type].grain * 0.048f *
                              vg_air_band((float)x + scroll, 7, 471u);
                if (type == 5)
                    warped += type_map[type].self * 19.0f *
                              fm_sinf(0.040f * (float)x + phase * 0.7f);
                warped = fm_maxf(warped, 0.0f);
                n = (int)(warped / (float)period);
                {
                    const float arrival = warped - (float)n * (float)period;
                    const float width = type == 0 ? 13.0f : type == 4 ? 12.0f : 19.0f;
                    const float repeat_gain = 1.0f - (1.0f - repeats) *
                        fm_clampf((float)n * 0.18f, 0.0f, 1.0f);
                    if (arrival < width) {
                        env = gain * (1.0f - arrival / width) * repeat_gain;
                        if (type == 5)
                            env *= 0.73f + 0.27f *
                                   (0.5f + 0.5f * fm_sinf(0.065f * (float)x + phase));
                    }
                }
                if (type_map[type].ping > 0.0f)
                    center += (n & 1) ? 8 : -8;
                else if (type == 1)
                    center += p == 2 ? 5 : p == 0 ? -3 : 0;
                {
                    const uint32_t h0 = vg_hash((uint32_t)((x + p * 11 + scroll_step) * 89));
                    const uint32_t h1 = vg_hash((uint32_t)((x + p * 11 + scroll_step + 1) * 89));
                    const float rough = ((float)(h0 % 23u) - 11.0f) * (1.0f - scroll_frac) +
                                        ((float)(h1 % 23u) - 11.0f) * scroll_frac;
                    const float smooth = 10.0f * vg_air_band((float)x + scroll, 4,
                                                               (uint32_t)(903 + p * 197));
                    /* The delay loop's actual tone coefficient influences
                     * edge detail; Grain adds its own high-rate jitter. */
                    const float sharp = type_map[type].grain > 0.0f ? 0.98f :
                                        fm_clampf(type_map[type].tone + 0.07f, 0.20f, 0.98f);
                    noise = rough * sharp + smooth * (1.0f - sharp);
                }
            }
            {
                const int amp = (int)(noise * env * (type == 4 ? 1.40f : 1.28f));
                const int yy = vg_max(18, vg_min(56, center + amp));
                uint16_t ink = vg_dim(VL_AIR, p == 2 ? 1.0f : 0.42f + 0.20f * (float)p);
                if (drive > 0.0f && (p == 2 || (p == 1 && vg_abs(amp) > 6)))
                    ink = driven_ink;
                if (x > 10 && env > 0.0f)
                    cv_line(px, py, x, yy, ink);
                else if (x > 10 && pos >= 0 && p == 1 && (x % 3) == 0)
                    cv_rect(x, center, 1, 1, vg_dim(VL_AIR, 0.24f));
                if (type == 4 && env > 0.13f && (vg_hash((uint32_t)(x * 17 + p * 331)) & 7u) == 0u)
                    cv_rect(x, vg_max(18, vg_min(56, yy + ((x & 1) ? 3 : -3))),
                            1, 1, vg_dim(LK_WHITE, 0.78f)); /* granular flecks */
                px = x; py = yy;
            }
        }
    }
    for (x = 18, n = 0; x <= 229; x += period, n++) {
        const int mark = type == 2 ? ((n & 1) ? 48 : 22) : 21;
        cv_rect(x, mark, 1, 4, type == 2 && (n & 1) ? LK_WHITE : VL_AIR);
        cv_rect(x, 58, 1, 2, LK_LABEL);
    }
}
static void vl_air(void)
{
    const float repeats = vl_norm(P_DLY_REPEATS), wet_delay = vl_norm(P_DLY_MIX);
    const float wet_reverb = vl_norm(P_REV_MIX), decay = vl_norm(P_REV_LEN);
    const float damping = vl_norm(P_REV_DAMP);
    const float drive = vl_norm(P_DLY_DRIVE);
    const int period = 19 + (int)(vl_norm(P_DLY_TIME) * 34.0f);
    const int hold = proj.par[P_REV_LEN] >= 100;
    const uint32_t now = plat_ms();
    const float scroll = vl_air_scroll(now);
    int x;
    cv_text(9, 1, &FONT_XS, "DELAY / ARRIVALS", LK_LABEL);
    cv_text(9, 64, &FONT_XS, hold ? "REVERB / HELD FIELD" : "REVERB / CODA", LK_LABEL);
    vg_air_delay(wet_delay, repeats, drive, period, scroll, now);
    cv_rect(8, 94, 224, 1, LK_RULE);
    for (x = 10; x <= 230; x += 22)
        cv_rect(x, 77, 1, 35, LK_RULE);
    vg_air_reverb(wet_reverb, decay, damping, hold, scroll, now);
    cv_rect(8, 118, 224, 1, LK_RULE);
}
/* Setup 2: the original planet remains fixed at 76px in the center.
 * L and R are wavy orbital traces, separated symmetrically about a fixed
 * orbital reference radius outside the planet. Width moves them toward or
 * away from that reference; it does not enlarge the planet. */
static void vl_planet(void)
{
    const int cx = 120, cy = 65, r = 38;
    const int tuning = (unsigned)proj.par[P_TUNING] < DR_NTUNING ? proj.par[P_TUNING] : DR_TUN_12;
    const int n = dr_tun_n(tuning);
    const float shift = 1200.0f * fm_log2f(1.0f + (float)proj.par[P_TUNE] / 4400.0f);
    const float stereo = vl_norm(P_WIDTH);
    const float orbit = (float)r + 25.0f;
    const float phase = (float)(plat_ms() % 6000u) * (6.2831853f / 6000.0f);
    static const uint16_t layers[5] = {RGB(246, 208, 62), RGB(242, 156, 47),
                                       RGB(225, 88, 43), RGB(173, 47, 39),
                                       RGB(106, 29, 36)};
    int k, i;
    /* One fixed orbit reference: a small station marker to the right.
     * Both channels start near its radius and separate in opposite
     * directions as Stereo widens. */
    cv_rect(cx + (int)orbit, cy - 3, 1, 7, LK_LABEL);
    cv_rect(cx + (int)orbit - 3, cy, 7, 1, LK_LABEL);
    for (k = 0; k < 2; k++) {
        const float base = orbit + (k ? 1.0f : -1.0f) * (2.0f + 13.0f * stereo);
        int strand;
        for (strand = 0; strand < 3; strand++) {
            int j, px = -1, py = -1;
            const uint16_t ink = vl_bright(LK_DRONE[k], strand == 0 ? 0.92f :
                                            strand == 1 ? 0.55f : 0.27f);
            for (j = 0; j <= 128; j++) {
                const float a = 6.2831853f * (float)j / 128.0f;
                const float wiggle = (2.2f + 0.7f * (float)strand) *
                       fm_sinf(7.0f * a + phase + (float)k * 1.6f +
                               (float)strand * 0.55f);
                const float rr = base + (float)strand * 2.0f + wiggle;
                const int x = cx + (int)(rr * fm_sinf(a));
                const int y = cy + (int)(rr * fm_cosf(a));
                if (px >= 8 && px <= 231 && py >= 8 && py <= 115 &&
                    x >= 8 && x <= 231 && y >= 8 && y <= 115)
                    cv_line(px, py, x, y, ink);
                px = x; py = y;
            }
        }
        {
            const int lx = cx + (k ? 1 : -1) * (int)(base + 9.0f);
            if (lx > 14 && lx < 226)
                text_c(lx, cy - 4, &FONT_XS, k ? "R" : "L", LK_WHITE);
        }
    }
    lk_circle(cx, cy, r, LK_LABEL);
    for (k = 0; k < 5; k++) {
        const int rr = r - 5 - k * 6;
        int px = cx, py = cy - rr;
        for (i = 1; i <= 28; i++) {
            const float a = 3.14159265f * (float)i / 28.0f;
            const int x = cx + (int)((float)rr * fm_sinf(a));
            const int y = cy - (int)((float)rr * fm_cosf(a));
            cv_line(px, py, x, y, vl_bright(layers[k], 0.85f));
            px = x; py = y;
        }
    }
    cv_rect(cx, cy - r, 1, r * 2, LK_WHITE);
    for (i = -2; i <= 2; i++) {
        const int y = cy + i * 11;
        cv_line(cx - 30, y, cx - 2, y, vl_bright(LK_LABEL, 0.33f));
    }
    /* The actual interval positions of the selected tuning, as before. */
    for (k = 0; k < n; k++) {
        const float note = (float)dr_tun_c(tuning, k) + shift;
        lk_spoke(cx, cy, note, (float)r + 2.0f,
                 (float)r + (n > 24 ? 6.0f : 9.0f), 1,
                 k == 0 ? LK_WHITE : LK_LABEL);
    }
    {
        int32_t ax, ay;
        lk_polar(cx, cy, shift, (float)r + 18.0f, &ax, &ay);
        text_c(ax, ay - LK_B(FONT_XS) + 6, &FONT_XS, "A", LK_WHITE);
    }
}
