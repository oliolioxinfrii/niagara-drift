/* SPDX-License-Identifier: GPL-3.0-only */
/* GHOST batch 1, browser listening experiments.
 *
 * Included in drone.c as a single translation unit. Both processes consume
 * the already-audible, per-drone post-filter signal. Their outputs require
 * the two drones; neither changes the existing oscillators or sequencer.
 *
 * This file is deliberately NOT yet cost-optimized for the FM-1 hardware.
 */
#define GH_BODY_MODES 8u

typedef struct {
    float re, im, c, s, pole, injection;
} ghost_body_mode_t;
static ghost_body_mode_t gh_body[GH_BODY_MODES];
static float gh_body_dc;
static float gh_terrain_lp, gh_terrain_dc;
static float gh_exp_power[2], gh_exp_gain[2] = {6.0f, 6.0f};

/* Pitch is the performed MIDI note, not the raw integer pitch parameter. */
static void ghost_body_tune(void)
{
    static const float tau[GH_BODY_MODES] = {
        0.19f, 0.24f, 0.29f, 0.35f, 0.18f, 0.25f, 0.42f, 0.16f
    };
    float f[GH_BODY_MODES];
    const float a = 440.0f * fm_exp2f((dr_pitch[0] - 69.0f) / 12.0f);
    const float b = 440.0f * fm_exp2f((dr_pitch[1] - 69.0f) / 12.0f);
    unsigned i;
    f[0] = fm_fabsf(a - b);
    f[1] = a + b;
    f[2] = fm_fabsf(2.0f*a - b);
    f[3] = fm_fabsf(2.0f*b - a);
    f[4] = 2.0f*a + b;
    f[5] = a + 2.0f*b;
    f[6] = fm_fabsf(3.0f*a - 2.0f*b);
    f[7] = 0.727f * (a + b); /* mildly inharmonic cavity response */
    for (i = 0u; i < GH_BODY_MODES; ++i) {
        const float hz = fm_clampf(f[i], 25.0f, 2900.0f);
        const float turns = hz / DR_SR;
        const float p = fm_exp2f(-1.44269504088896f / (tau[i] * DR_SR));
        gh_body[i].c = fm_cosf(FM_TWO_PI * turns);
        gh_body[i].s = fm_sin_turns(turns);
        gh_body[i].pole = p;
        gh_body[i].injection = 3.0f * (1.0f - p);
    }
}
static float ghost_body_sample(float u, float v)
{
    float xy = u*v, excitation, out = 0.0f;
    unsigned i;
    /* Direct quadratic and cubic cross-products excite the cavity;
     * individual parent tones are never injected by themselves. */
    gh_body_dc += 0.00012f * (xy - gh_body_dc);
    excitation = (xy - gh_body_dc) + 0.55f * (u*u*v + v*v*u);
    excitation = 0.82f * fm_tanhf(10.0f * excitation);
    for (i = 0u; i < GH_BODY_MODES; ++i) {
        ghost_body_mode_t *m = &gh_body[i];
        const float re = m->re, im = m->im, p = m->pole;
        m->re = p * (m->c * re - m->s * im) + m->injection * excitation;
        m->im = p * (m->s * re + m->c * im);
        /* Give low combination modes greater weight without removing the
         * upper resonance and odd cavity resonances. */
        out += (i < 4u ? 0.31f : 0.18f) * m->re;
    }
    return out;
}

/* Curved 2-D wave-terrain surface. It is intentionally not separable into
 * a product of two 1-D wave-shapers. Bend responds to the parent energy
 * balance, so long-term changes traverse somewhat different contours. */
static float ghost_terrain_height(float x, float y, float bend)
{
    return 0.64f * fm_sinf(5.4f*x + 3.1f*y + 3.7f*x*y + bend*x*x)
         + 0.34f * fm_cosf(4.0f*x - 5.3f*y + 2.9f*x*y)
         + 0.24f * fm_sinf(6.0f*(x*x-y*y) + 3.6f*x*y);
}
static float ghost_terrain_sample(float u, float v)
{
    const float x = fm_tanhf(2.8f*u), y = fm_tanhf(2.8f*v);
    const float p0 = ghost.power[0], p1 = ghost.power[1];
    const float bend = 1.1f + 0.75f * (p0-p1)/(p0+p1+0.005f);
    /* Mixed finite difference: F(x,y)-F(x,0)-F(0,y)+F(0,0).
     * Exactly zero if either parent is zero, unlike ordinary waveshaping.
     * Subtracts any contributions attributable to either drone alone. */
    const float a = ghost_terrain_height(x,y,bend);
    const float b = ghost_terrain_height(x,0.0f,bend);
    const float c = ghost_terrain_height(0.0f,y,bend);
    const float d = 0.34f;  /* F(0,0) = 0.34 (constant for all bend). */
    const float cross = 0.48f * (a - b - c + d);
    gh_terrain_lp += 0.48f * (cross - gh_terrain_lp);
    gh_terrain_dc += 0.00018f * (gh_terrain_lp - gh_terrain_dc);
    return gh_terrain_lp - gh_terrain_dc;
}
static void ghost_batch1_reset(void)
{
    unsigned i;
    dr_memset(gh_body, 0, sizeof gh_body);
    gh_body_dc = gh_terrain_lp = gh_terrain_dc = 0.0f;
    for (i = 0u; i < 2u; ++i) {
        gh_exp_power[i] = 0.0f;
        gh_exp_gain[i] = 6.0f;
    }
}
static void ghost_batch1_levels(float body, float terrain)
{
    /* RMS detectors are independent: a quiet mode never inherits a gain
     * calibrated for another algorithm. All modes run for seamless A/B. */
    gh_exp_power[0] += 0.00018f * (body*body - gh_exp_power[0]);
    gh_exp_power[1] += 0.00018f * (terrain*terrain - gh_exp_power[1]);
}
static void ghost_batch1_gain_update(uint32_t n)
{
    const float parent = fm_sqrtf(ghost.power[0] + ghost.power[1]);
    const float slew = fm_minf((float)n / (0.55f * DR_SR), 1.0f);
    unsigned i;
    for (i = 0u; i < 2u; ++i) {
        /* At maximum GHOST level, aim around 40% of parents' combined RMS,
         * while respecting source dependence and bounding gain. */
        const float rms = fm_sqrtf(gh_exp_power[i] + 1e-6f);
        const float target = fm_clampf(0.40f * parent / rms, 0.65f, 32.0f);
        gh_exp_gain[i] += slew * (target - gh_exp_gain[i]);
        gh_exp_power[i] = fm_flush(gh_exp_power[i]);
    }
    gh_body_dc = fm_flush(gh_body_dc);
    gh_terrain_lp = fm_flush(gh_terrain_lp);
    gh_terrain_dc = fm_flush(gh_terrain_dc);
    for (i = 0u; i < GH_BODY_MODES; ++i) {
        gh_body[i].re = fm_flush(gh_body[i].re);
        gh_body[i].im = fm_flush(gh_body[i].im);
    }
}
static float ghost_batch1_level(float x, unsigned which)
{
    float a = fm_clampf(x * gh_exp_gain[which], -1.0f, 1.0f);
    return a * (1.0f - (1.0f/3.0f) * a*a);
}
