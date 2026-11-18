/* SPDX-License-Identifier: GPL-3.0-only */
/* Experimental GHOST batch 2: selectively heterodyned partials and
 * reciprocal spectral-envelope cross synthesis. Included by drone.c.
 * Both modes are dependent on two post-filter audible parent waveforms.
 * Expensive exploratory implementation; no FM-1 hardware approval.
 */
#define GH_HET_PARTIALS 3u
#define GH_XS_BANDS 12u

typedef struct {
    float re, im, c, s, r, inject;
} ghost_complex_partial_t;
static ghost_complex_partial_t gh_het[2][GH_HET_PARTIALS];
static float gh_het_dc, gh_het_lp;

typedef struct {
    float ic1, ic2, env, a1, a2, a3;
} ghost_xs_band_t;
static ghost_xs_band_t gh_xs[2][GH_XS_BANDS];
static float gh_xs_lp, gh_xs_dc;

/* Independent level matching; the previous FFT test had no corresponding
 * source-relative calibration and was difficult to hear. */
static float gh_b2_power[2], gh_b2_gain[2];

static void ghost_batch2_reset(void)
{
    dr_memset(gh_het, 0, sizeof gh_het);
    dr_memset(gh_xs, 0, sizeof gh_xs);
    gh_het_dc = gh_het_lp = gh_xs_lp = gh_xs_dc = 0.0f;
    gh_b2_power[0] = gh_b2_power[1] = 0.0f;
    gh_b2_gain[0] = gh_b2_gain[1] = 4.0f;
}

static void ghost_batch2_tune(void)
{
    const float f0 = 440.0f * fm_exp2f((dr_pitch[0] - 69.0f) / 12.0f);
    const float f1 = 440.0f * fm_exp2f((dr_pitch[1] - 69.0f) / 12.0f);
    unsigned i, p;
    for (i = 0; i < 2u; ++i) {
        const float fundamental = i ? f1 : f0;
        for (p = 0; p < GH_HET_PARTIALS; ++p) {
            ghost_complex_partial_t *s = &gh_het[i][p];
            const float hz = fm_clampf(fundamental * (float)(p + 1u), 25.0f, 4500.0f);
            /* 4%-width partial filters, at least 7 Hz wide. Positive
             * frequency complex resonators reject opposite sidebands. */
            const float bw = fm_maxf(7.0f, hz * 0.045f);
            const float r = fm_exp2f(-9.06472028f * bw / DR_SR);
            const float turns = hz / DR_SR;
            s->c = fm_cosf(FM_TWO_PI * turns);
            s->s = fm_sin_turns(turns);
            s->r = r;
            s->inject = 1.0f - r;
        }
    }
}

static float ghost_heterodyne_sample(float u, float v)
{
    /* Selected complex cross-products x_i * conj(y_j) retain DIFFERENCE
     * sidebands, unlike real multiplication (which also emits sums).
     * These resonators have no self-excitation: each receives only its
     * own parent input, and output requires a product of both states. */
    static const uint8_t pairs[5][2] = {
        {0,0}, {1,0}, {0,1}, {2,1}, {1,2}
    };
    static const float weights[5] = {1.0f, 0.64f, 0.64f, 0.43f, 0.43f};
    float out = 0.0f;
    unsigned i, j;
    for (i = 0; i < 2u; ++i) {
        const float input = i ? v : u;
        for (j = 0; j < GH_HET_PARTIALS; ++j) {
            ghost_complex_partial_t *s = &gh_het[i][j];
            const float oldre = s->re, oldim = s->im;
            s->re = s->r * (s->c * oldre - s->s * oldim) + s->inject * input;
            s->im = s->r * (s->s * oldre + s->c * oldim);
        }
    }
    for (i = 0; i < 5u; ++i) {
        const ghost_complex_partial_t *a = &gh_het[0][pairs[i][0]];
        const ghost_complex_partial_t *b = &gh_het[1][pairs[i][1]];
        out += weights[i] * (a->re * b->re + a->im * b->im);
    }
    out *= 4.0f;
    gh_het_dc += 0.00145f * (out - gh_het_dc); /* remove near-unison DC */
    gh_het_lp += 0.37f * (out - gh_het_dc - gh_het_lp);
    return gh_het_lp;
}

/* A 12-band spectral cross-synth, rather than the previous bin-gated
 * FFT mode: the excitation of each narrow band is reconstructed from
 * the original audio, and the OTHER drone's band-envelope redistributes
 * its energy. This produces sound across different fundamental pitches
 * instead of demanding identical bins or identical harmonics. */
static void ghost_xs_prepare(void)
{
    static const float hz[GH_XS_BANDS] = {
        35.0f, 51.0f, 75.0f, 111.0f, 163.0f, 239.0f,
        351.0f, 516.0f, 758.0f, 1114.0f, 1637.0f, 2405.0f
    };
    unsigned i, j;
    for (i = 0; i < 2u; ++i)
        for (j = 0; j < GH_XS_BANDS; ++j) {
            const float g = fm_tanf(FM_PI * hz[j] / DR_SR);
            const float k = 0.95f; /* about 1.05 Q: overlapping logarithmic bands */
            ghost_xs_band_t *s = &gh_xs[i][j];
            s->a1 = 1.0f / (1.0f + g * (g + k));
            s->a2 = g * s->a1;
            s->a3 = g * s->a2;
        }
}
static float ghost_xs_filter(ghost_xs_band_t *s, float x)
{
    /* Topology-preserving SVF bandpass; its phase is supplied by the
     * actual drone input, not an oscillator generated for GHOST. */
    const float v3 = x - s->ic2;
    const float v1 = s->a1 * s->ic1 + s->a2 * v3;
    const float v2 = s->ic2 + s->a2 * s->ic1 + s->a3 * v3;
    s->ic1 = 2.0f * v1 - s->ic1;
    s->ic2 = 2.0f * v2 - s->ic2;
    s->env += 0.0014f * (fm_fabsf(v1) - s->env);
    return v1;
}
static float ghost_cross_synthesis_sample(float u, float v)
{
    float bands[2][GH_XS_BANDS], total[2] = {0.0f, 0.0f};
    float result = 0.0f, gate0, gate1;
    unsigned j;
    for (j = 0; j < GH_XS_BANDS; ++j) {
        bands[0][j] = ghost_xs_filter(&gh_xs[0][j], u);
        bands[1][j] = ghost_xs_filter(&gh_xs[1][j], v);
        total[0] += gh_xs[0][j].env;
        total[1] += gh_xs[1][j].env;
    }
    /* The broad pedestal prevents near-disjoint spectra from cancelling
     * completely. It is multiplied by the OTHER drone's measured energy,
     * so a lone parent can never sustain a GHOST signal. */
    gate0 = total[0] / (total[0] + 0.0015f);
    gate1 = total[1] / (total[1] + 0.0015f);
    for (j = 0; j < GH_XS_BANDS; ++j) {
        const float e0 = gh_xs[0][j].env, e1 = gh_xs[1][j].env;
        const float avg0 = total[0] * (1.0f / GH_XS_BANDS) + 1e-5f;
        const float avg1 = total[1] * (1.0f / GH_XS_BANDS) + 1e-5f;
        const float color0 = fm_clampf(0.24f + 0.58f * (e0 / avg0), 0.24f, 2.7f);
        const float color1 = fm_clampf(0.24f + 0.58f * (e1 / avg1), 0.24f, 2.7f);
        result += (bands[0][j] * color1 * gate1 +
                   bands[1][j] * color0 * gate0) * 0.14f;
    }
    gh_xs_lp += 0.42f * (result - gh_xs_lp);
    gh_xs_dc += 0.00025f * (gh_xs_lp - gh_xs_dc);
    return gh_xs_lp - gh_xs_dc;
}

static void ghost_batch2_levels(float hetero, float cross)
{
    gh_b2_power[0] += 0.0002f * (hetero*hetero - gh_b2_power[0]);
    gh_b2_power[1] += 0.0002f * (cross*cross - gh_b2_power[1]);
}

static void ghost_batch2_gain_update(uint32_t n)
{
    const float parent = fm_sqrtf(ghost.power[0] + ghost.power[1]);
    const float slew = fm_minf((float)n / (0.45f * DR_SR), 1.0f);
    unsigned i;
    for (i = 0u; i < 2u; ++i) {
        const float rms = fm_sqrtf(gh_b2_power[i] + 1e-7f);
        const float desired = (i == 0u ? 0.56f : 0.48f) * parent / rms;
        const float target = fm_clampf(desired, 0.15f, i == 0u ? 96.0f : 48.0f);
        gh_b2_gain[i] += slew * (target - gh_b2_gain[i]);
        gh_b2_power[i] = fm_flush(gh_b2_power[i]);
    }
    gh_het_dc = fm_flush(gh_het_dc);
    gh_het_lp = fm_flush(gh_het_lp);
    gh_xs_dc = fm_flush(gh_xs_dc);
    gh_xs_lp = fm_flush(gh_xs_lp);
    for (i = 0u; i < GH_HET_PARTIALS; ++i) {
        gh_het[0][i].re = fm_flush(gh_het[0][i].re);
        gh_het[0][i].im = fm_flush(gh_het[0][i].im);
        gh_het[1][i].re = fm_flush(gh_het[1][i].re);
        gh_het[1][i].im = fm_flush(gh_het[1][i].im);
    }
    for (i = 0u; i < GH_XS_BANDS; ++i) {
        gh_xs[0][i].ic1 = fm_flush(gh_xs[0][i].ic1);
        gh_xs[0][i].ic2 = fm_flush(gh_xs[0][i].ic2);
        gh_xs[1][i].ic1 = fm_flush(gh_xs[1][i].ic1);
        gh_xs[1][i].ic2 = fm_flush(gh_xs[1][i].ic2);
        gh_xs[0][i].env = fm_flush(gh_xs[0][i].env);
        gh_xs[1][i].env = fm_flush(gh_xs[1][i].env);
    }
}
static float ghost_batch2_level(float x, unsigned which)
{
    const float a = fm_clampf(x * gh_b2_gain[which], -1.0f, 1.0f);
    return a * (1.0f - a*a / 3.0f);
}
