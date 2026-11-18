/* SPDX-License-Identifier: GPL-3.0-only */
/* GHOST listening blends. Included by drone.c after the five engines.
 *
 * Modes 5–12: parallel, source-dependent linear combinations.
 * Mode 13: complementary 2-pole LF/HF division of BODY/TERRAIN.
 * Mode 14: genuinely different serial excitation: the Wave Terrain
 *          signal (not Body's ordinary excitation) drives its OWN bank
 *          of eight damped Body resonators. No third oscillator.
 *
 * Separate slow per-blend calibration compensates for correlated signals,
 * but preserves component ratios within each blend. The individual engine
 * gain detectors continue to operate unmodified.
 */

static float gh_split_body_lp1, gh_split_body_lp2;
static float gh_split_terrain_lp1, gh_split_terrain_lp2;
typedef struct { float re, im; } gh_excite_res_t;
static gh_excite_res_t gh_excite_body[GH_BODY_MODES];

static float gh_blend_power[DR_NGHOSTMODES], gh_blend_gain[DR_NGHOSTMODES];

static void ghost_blends_reset(void)
{
    uint32_t i;
    gh_split_body_lp1 = gh_split_body_lp2 = 0.0f;
    gh_split_terrain_lp1 = gh_split_terrain_lp2 = 0.0f;
    dr_memset(gh_excite_body, 0, sizeof gh_excite_body);
    dr_memset(gh_blend_power, 0, sizeof gh_blend_power);
    for (i = 0u; i < DR_NGHOSTMODES; ++i)
        gh_blend_gain[i] = 1.0f;
}

/* Two-pole complementary crossover, 350 Hz. Each pathway is separate:
 * low(BODY)=L^2(BODY); high(TERRAIN)=(1-L)^2(TERRAIN).
 * For any single source signal, low+high is a complementary pair.
 * Here the source signals differ; the design is a spectral division,
 * not a disguised broadband sum. */
static float ghost_blend_split_sample(float body, float terrain)
{
    const float a = 0.0486f;
    gh_split_body_lp1 += a * (body - gh_split_body_lp1);
    gh_split_body_lp2 += a * (gh_split_body_lp1 - gh_split_body_lp2);
    gh_split_terrain_lp1 += a * (terrain - gh_split_terrain_lp1);
    gh_split_terrain_lp2 += a * (gh_split_terrain_lp1 - gh_split_terrain_lp2);
    return 0.90f * gh_split_body_lp2
         + 0.90f * (terrain - 2.0f * gh_split_terrain_lp1
                             + gh_split_terrain_lp2);
}

/* An independent resonant object excited by Wave Terrain instead of
 * the Body engine's xy + cubic cross-products. Shared tuned coefficients,
 * separate states; neither the Body output nor its resonator memory is
 * altered by this processing path. */
static float ghost_terrain_excites_body_sample(float raw_terrain)
{
    const float exc = 0.82f * fm_tanhf(14.0f * raw_terrain);
    float out = 0.0f;
    uint32_t i;
    for (i = 0u; i < GH_BODY_MODES; ++i) {
        const ghost_body_mode_t *tune = &gh_body[i];
        gh_excite_res_t *m = &gh_excite_body[i];
        const float re = m->re, im = m->im;
        m->re = tune->pole * (tune->c * re - tune->s * im)
              + tune->injection * exc;
        m->im = tune->pole * (tune->s * re + tune->c * im);
        out += (i < 4u ? 0.31f : 0.18f) * m->re;
    }
    return out;
}

/* Fill fifteen modes from exactly five original parent-derived signals.
 * No synthesis engine is evaluated more than once per audio sample.
 * Inputs are already individually calibrated, bounded, and wave-shaped;
 * the resulting blended modes get their own *slow* output levelling below.
 *
 * B=Body, T=Terrain, H=Heterodyne, C=Cross-Synth, F=Feedback.
 */
static void ghost_blends_fill(float *m, float b, float t, float h, float c,
                              float f, float terrain_excitation)
{
    uint32_t i;
    m[DR_GHOST_BODY] = b;
    m[DR_GHOST_TERRAIN] = t;
    m[DR_GHOST_HETERODYNE] = h;
    m[DR_GHOST_CROSS_SYNTH] = c;
    m[DR_GHOST_FEEDBACK] = f;

    m[DR_GHOST_CROSS_BODY] = 0.70f*c + 0.30f*b;
    m[DR_GHOST_CROSS_TERRAIN] = 0.70f*c + 0.30f*t;
    m[DR_GHOST_CROSS_FEEDBACK] = 0.70f*c + 0.30f*f;
    m[DR_GHOST_TERRAIN_BODY] = 0.60f*t + 0.40f*b;
    m[DR_GHOST_BODY_FEEDBACK] = 0.65f*b + 0.35f*f;
    m[DR_GHOST_CROSS_HETERODYNE] = 0.70f*c + 0.30f*h;
    m[DR_GHOST_CROSS_BODY_FEEDBACK] = 0.55f*c + 0.30f*b + 0.15f*f;
    m[DR_GHOST_CROSS_TERRAIN_BODY] = 0.50f*c + 0.30f*t + 0.20f*b;
    m[DR_GHOST_SPLIT_BODY_TERRAIN] = ghost_blend_split_sample(b, t);
    m[DR_GHOST_TERRAIN_EXCITES_BODY] = terrain_excitation;

    for (i = DR_GHOST_CROSS_BODY; i < DR_NGHOSTMODES; ++i) {
        const float value = m[i];
        gh_blend_power[i] += 0.00018f * (value*value - gh_blend_power[i]);
        /* Calibrate the whole combination, not each already-normalized
         * component separately. This maintains the intended weight ratios.
         * Every modal output, including the source-dependent special cases,
         * still falls silent when neither parent is playing. */
        {
            const float x = fm_clampf(value * gh_blend_gain[i], -1.0f, 1.0f);
            m[i] = x * (1.0f - x*x*(1.0f/3.0f));
        }
    }
}

static void ghost_blends_gain_update(uint32_t n)
{
    const float parent = fm_sqrtf(ghost.power[0] + ghost.power[1]);
    const float slew = fm_minf((float)n/(0.65f*DR_SR), 1.0f);
    uint32_t i;
    for (i = DR_GHOST_CROSS_BODY; i < DR_NGHOSTMODES; ++i) {
        const float rms = fm_sqrtf(gh_blend_power[i] + 1e-6f);
        /* Unlike a limiter, this only tracks over ~650 ms and can amplify
         * a quiet pair by at most 3x. No automatic makeup if the parents
         * themselves stop, and no more than 1.5x when very loud. */
        const float desired = fm_clampf(0.53f * parent / rms, 0.48f, 3.0f);
        gh_blend_gain[i] += slew * (desired - gh_blend_gain[i]);
        gh_blend_power[i] = fm_flush(gh_blend_power[i]);
    }
    gh_split_body_lp1 = fm_flush(gh_split_body_lp1);
    gh_split_body_lp2 = fm_flush(gh_split_body_lp2);
    gh_split_terrain_lp1 = fm_flush(gh_split_terrain_lp1);
    gh_split_terrain_lp2 = fm_flush(gh_split_terrain_lp2);
    for (i = 0u; i < GH_BODY_MODES; ++i) {
        gh_excite_body[i].re = fm_flush(gh_excite_body[i].re);
        gh_excite_body[i].im = fm_flush(gh_excite_body[i].im);
    }
}
