/* SPDX-License-Identifier: GPL-3.0-only */
/* GHOST mode 5: Cross-Synth (70%) + nonlinear resonant Body (30%).
 * Selective port of Niagara Drift's earlier listening experiment:
 * ghost_batch1.c, ghost_batch2.c, and ghost_blends.c. Only the two
 * engines needed for the 5 -> 14 morph are present.
 *
 * The 12-band cross-synth always retains two independently tracked
 * filter banks. The hybrid renderer reuses mode 14's eight resonators
 * with a continuously morphed excitation. The reference renderer
 * retains an independent body bank for an ordinary output crossfade.
 * No synthesis state is accessed by CPU0 while CPU1 executes its job.
 */
#define GH5_BANDS 12u
typedef struct {
    float ic1, ic2, env, a1, a2, a3;
} gh5_band_t;
static gh5_band_t gh5_bands[2][GH5_BANDS];
static float gh5_xs_lp, gh5_xs_dc, gh5_body_dc;
static float gh5_b_power, gh5_c_power, gh5_mix_power;
static float gh5_b_gain = 1.0f, gh5_c_gain = 4.0f, gh5_mix_gain = 1.0f;
#if OM_GHOST_BLEND_REFERENCE
static float gh5_r[GH14_N][2];   /* independent bank only for A/B reference */
#endif

/* Coefficients computed once at engine initialization (not on each mute).
 * Frequencies and Q come from the original 12-band mode 5. */
static void gh5_prepare(void)
{
    static const float hz[GH5_BANDS] = {
        35.0f, 51.0f, 75.0f, 111.0f, 163.0f, 239.0f,
        351.0f, 516.0f, 758.0f, 1114.0f, 1637.0f, 2405.0f
    };
    unsigned i, j;
    for (i = 0u; i < 2u; ++i)
        for (j = 0u; j < GH5_BANDS; ++j) {
            const float g = fm_tanf(FM_PI * hz[j] / DR_SR);
            const float k = 0.95f;
            gh5_band_t *s = &gh5_bands[i][j];
            s->a1 = 1.0f / (1.0f + g * (g + k));
            s->a2 = g * s->a1;
            s->a3 = g * s->a2;
        }
}
static void gh5_reset(void)
{
    unsigned i, j;
    for (i = 0u; i < 2u; ++i)
        for (j = 0u; j < GH5_BANDS; ++j) {
            gh5_band_t *s = &gh5_bands[i][j];
            s->ic1 = s->ic2 = s->env = 0.0f; /* preserve a1,a2,a3 */
        }
#if OM_GHOST_BLEND_REFERENCE
    dr_memset(gh5_r, 0, sizeof gh5_r);
#endif
    gh5_xs_lp = gh5_xs_dc = gh5_body_dc = 0.0f;
    gh5_b_power = gh5_c_power = gh5_mix_power = 0.0f;
    gh5_b_gain = gh5_mix_gain = 1.0f;
    gh5_c_gain = 4.0f;
}
/* Original mode-5 body excitation, separated from its eight resonators.
 * Both parents contribute multiplicatively; no third independent voice. */
static float gh5_body_excitation(float u, float v)
{
    const float xy = u * v;
    const float cross = u * u * v + v * v * u;
    gh5_body_dc += 0.00012f * (xy - gh5_body_dc);
    return 0.82f * fm_tanhf(10.0f * ((xy - gh5_body_dc) + 0.55f * cross));
}
#if OM_GHOST_BLEND_REFERENCE
/* Reference mode 5 uses a separate bank, with exactly the same modal
 * coefficients as the terrain-excited body (shared tuning, not state). */
static float gh5_body_resonate(float exc)
{
    float out = 0.0f;
    unsigned i;
    for (i = 0u; i < GH14_N; ++i) {
        const gh14_res_t *tune = &gh14_body[i];
        float *m = gh5_r[i];
        const float re = m[0], im = m[1];
        m[0] = tune->pole * (tune->c * re - tune->s * im)
             + tune->injection * exc;
        m[1] = tune->pole * (tune->s * re + tune->c * im);
        out += (i < 4u ? 0.31f : 0.18f) * m[0];
    }
    return out;
}
#endif
static float gh5_filter(gh5_band_t *s, float x)
{
    const float v3 = x - s->ic2;
    const float v1 = s->a1 * s->ic1 + s->a2 * v3;
    const float v2 = s->ic2 + s->a2 * s->ic1 + s->a3 * v3;
    s->ic1 = 2.0f * v1 - s->ic1;
    s->ic2 = 2.0f * v2 - s->ic2;
    s->env += 0.0014f * (fm_fabsf(v1) - s->env);
    return v1;
}
/* Two 12-band SVF banks; each parent is shaped by the OTHER's measured
 * band energy. Unlike an FFT bin intersection, differing pitches survive. */
static float gh5_cross_synthesis(float u, float v)
{
    float bands[2][GH5_BANDS], total[2] = {0.0f, 0.0f};
    float result = 0.0f, gate0, gate1;
    unsigned j;
    for (j = 0u; j < GH5_BANDS; ++j) {
        bands[0][j] = gh5_filter(&gh5_bands[0][j], u);
        bands[1][j] = gh5_filter(&gh5_bands[1][j], v);
        total[0] += gh5_bands[0][j].env;
        total[1] += gh5_bands[1][j].env;
    }
    gate0 = total[0] / (total[0] + 0.0015f);
    gate1 = total[1] / (total[1] + 0.0015f);
    for (j = 0u; j < GH5_BANDS; ++j) {
        const float e0 = gh5_bands[0][j].env, e1 = gh5_bands[1][j].env;
        const float avg0 = total[0] * (1.0f / GH5_BANDS) + 1e-5f;
        const float avg1 = total[1] * (1.0f / GH5_BANDS) + 1e-5f;
        const float color0 = fm_clampf(0.24f + 0.58f * (e0 / avg0), 0.24f, 2.7f);
        const float color1 = fm_clampf(0.24f + 0.58f * (e1 / avg1), 0.24f, 2.7f);
        result += (bands[0][j] * color1 * gate1 +
                   bands[1][j] * color0 * gate0) * 0.14f;
    }
    gh5_xs_lp += 0.42f * (result - gh5_xs_lp);
    gh5_xs_dc += 0.00025f * (gh5_xs_lp - gh5_xs_dc);
    return gh5_xs_lp - gh5_xs_dc;
}
static float gh5_shape(float x)
{
    x = fm_clampf(x, -1.0f, 1.0f);
    return x * (1.0f - x * x * (1.0f / 3.0f));
}
/* Mode 5's original independent body/cross and whole-combination slow
 * calibration: the 70:30 ratio is applied AFTER component levelling. */
static float gh5_mode5_output(float raw_body, float raw_cross)
{
    const float b = gh5_shape(raw_body * gh5_b_gain);
    const float c = gh5_shape(raw_cross * gh5_c_gain);
    const float mixed = 0.70f * c + 0.30f * b;
    gh5_b_power += 0.00018f * (raw_body * raw_body - gh5_b_power);
    gh5_c_power += 0.00020f * (raw_cross * raw_cross - gh5_c_power);
    gh5_mix_power += 0.00018f * (mixed * mixed - gh5_mix_power);
    return gh5_shape(mixed * gh5_mix_gain);
}
static void gh5_finish(uint32_t n)
{
    const float parent = fm_sqrtf(ghost.power[0] + ghost.power[1]);
    const float rb = fm_sqrtf(gh5_b_power + 1e-6f);
    const float rc = fm_sqrtf(gh5_c_power + 1e-7f);
    const float rm = fm_sqrtf(gh5_mix_power + 1e-6f);
    const float sb = fm_minf((float)n / (0.55f * DR_SR), 1.0f);
    const float sc = fm_minf((float)n / (0.45f * DR_SR), 1.0f);
    const float sm = fm_minf((float)n / (0.65f * DR_SR), 1.0f);
    const float db = fm_clampf(0.68f * parent / rb, 0.18f, 48.0f);
    const float dc = fm_clampf(0.48f * parent / rc, 0.15f, 48.0f);
    const float dm = fm_clampf(0.53f * parent / rm, 0.48f, 3.0f);
    gh5_b_gain += sb * (db - gh5_b_gain);
    gh5_c_gain += sc * (dc - gh5_c_gain);
    gh5_mix_gain += sm * (dm - gh5_mix_gain);
    gh5_b_power = fm_flush(gh5_b_power);
    gh5_c_power = fm_flush(gh5_c_power);
    gh5_mix_power = fm_flush(gh5_mix_power);
    gh5_xs_lp = fm_flush(gh5_xs_lp);
    gh5_xs_dc = fm_flush(gh5_xs_dc);
    gh5_body_dc = fm_flush(gh5_body_dc);
    for (unsigned i = 0u; i < 2u; ++i)
        for (unsigned j = 0u; j < GH5_BANDS; ++j) {
            gh5_band_t *s = &gh5_bands[i][j];
            s->ic1 = fm_flush(s->ic1);
            s->ic2 = fm_flush(s->ic2);
            s->env = fm_flush(s->env);
        }
#if OM_GHOST_BLEND_REFERENCE
    for (unsigned i = 0u; i < GH14_N; ++i) {
        gh5_r[i][0] = fm_flush(gh5_r[i][0]);
        gh5_r[i][1] = fm_flush(gh5_r[i][1]);
    }
#endif
}
