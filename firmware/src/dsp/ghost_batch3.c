/* SPDX-License-Identifier: GPL-3.0-only */
/* GHOST Batch 3 -- browser-only comparative DSP experiments.
 *
 * 5: time-varying, bidirectional LIVE CONVOLUTION. In each 512-sample
 *    block, each performed drone is convolved with a 512-tap impulse
 *    response captured from the other drone. Real overlap-save FFT
 *    convolution (not FFT-bin intermodulation) guarantees that each
 *    output is an actual time-domain convolution with a changing IR.
 * 6: COUPLED FEEDBACK. Two driven delay networks cross-modulate their
 *    feedback coefficients without transmitting a source directly into
 *    the opposite network. Only their cross-dependent response is output.
 *
 * Included by drone.c, as with Batches 1 and 2; intentionally not
 * memory/CPU-optimized for the physical FM-1. No independent oscillator.
 */
#define GH_CV_B 512u
#define GH_CV_N 1024u
#define GH_CV_MASK (GH_CV_N - 1u)
#define GH_FB_RING 4096u
#define GH_FB_MASK (GH_FB_RING - 1u)

typedef struct { float re, im; } gh_cv_complex_t;
static float gh_cv_previous[2][GH_CV_B], gh_cv_current[2][GH_CV_B];
static float gh_cv_kernel[2][GH_CV_B], gh_cv_output[GH_CV_B];
static gh_cv_complex_t gh_cv_src[2][GH_CV_N], gh_cv_ir[2][GH_CV_N];
static uint32_t gh_cv_position;

static float gh_fb_line[2][GH_FB_RING];
static uint32_t gh_fb_write;
static float gh_fb_lp, gh_fb_dc;

static float gh_b3_power[2], gh_b3_gain[2];

/* Complex radix-2 FFT, forward unscaled and inverse scaled by 1/N.
 * Twiddles are evaluated once per stage, not in the inner sample loop. */
static void ghost_cv_fft(gh_cv_complex_t *v, int inverse)
{
    uint32_t i, j, length;
    for (i = 1u, j = 0u; i < GH_CV_N; ++i) {
        uint32_t bit = GH_CV_N >> 1u;
        while (j & bit) { j ^= bit; bit >>= 1u; }
        j ^= bit;
        if (i < j) {
            const gh_cv_complex_t tmp = v[i];
            v[i] = v[j]; v[j] = tmp;
        }
    }
    for (length = 2u; length <= GH_CV_N; length <<= 1u) {
        const float turns = (inverse ? 1.0f : -1.0f) / (float)length;
        const float wr0 = fm_cosf(FM_TWO_PI * turns);
        const float wi0 = fm_sin_turns(turns);
        uint32_t start;
        for (start = 0u; start < GH_CV_N; start += length) {
            float wr = 1.0f, wi = 0.0f;
            for (j = 0u; j < (length >> 1u); ++j) {
                const uint32_t a = start + j, b = a + (length >> 1u);
                const float br = wr * v[b].re - wi * v[b].im;
                const float bi = wr * v[b].im + wi * v[b].re;
                const float ar = v[a].re, ai = v[a].im;
                const float nw = wr * wr0 - wi * wi0;
                wi = wr * wi0 + wi * wr0;
                wr = nw;
                v[a].re = ar + br;
                v[a].im = ai + bi;
                v[b].re = ar - br;
                v[b].im = ai - bi;
            }
        }
    }
    if (inverse) {
        for (i = 0u; i < GH_CV_N; ++i) {
            v[i].re *= 1.0f / (float)GH_CV_N;
            v[i].im *= 1.0f / (float)GH_CV_N;
        }
    }
}

/* Render the just-completed source block against the other source's
 * recently recorded time-domain impulse response. Two 512-tap causal
 * convolutions are overlap-saved using a 1024-point FFT.
 *
 * IR weighting decays as (1 - tap / 512)^2, which gives a broad enough
 * spectrum to avoid the prior sparse-bin mode's near-total cancellations.
 * A 30%-per-frame IR update preserves temporal memory and reduces jumps.
 * Captured IR is NEVER normalized independently of the donor level:
 * when either parent stops, its IR fades and the GHOST eventually stops.
 */
static void ghost_cv_process(void)
{
    uint32_t source, i;
    for (source = 0u; source < 2u; ++source) {
        float mean = 0.0f;
        for (i = 0u; i < GH_CV_B; ++i)
            mean += gh_cv_current[source][i];
        mean *= 1.0f / (float)GH_CV_B;
        for (i = 0u; i < GH_CV_B; ++i) {
            const float t = (float)i / (float)GH_CV_B;
            const float taper = (1.0f - t) * (1.0f - t);
            const float recent = gh_cv_current[source][GH_CV_B - 1u - i];
            const float new_ir = (recent - 0.7f * mean) * taper * (1.0f / 32.0f);
            gh_cv_kernel[source][i] = 0.70f * gh_cv_kernel[source][i] +
                                      0.30f * new_ir;
            gh_cv_ir[source][i] = (gh_cv_complex_t){ gh_cv_kernel[source][i], 0.0f };
            gh_cv_ir[source][i + GH_CV_B] = (gh_cv_complex_t){ 0.0f, 0.0f };
            gh_cv_src[source][i] = (gh_cv_complex_t){ gh_cv_previous[source][i], 0.0f };
            gh_cv_src[source][i + GH_CV_B] =
                (gh_cv_complex_t){ gh_cv_current[source][i], 0.0f };
            gh_cv_previous[source][i] = gh_cv_current[source][i];
        }
        ghost_cv_fft(gh_cv_src[source], 0);
        ghost_cv_fft(gh_cv_ir[source], 0);
    }
    for (i = 0u; i < GH_CV_B; ++i)
        gh_cv_output[i] = 0.0f;
    for (source = 0u; source < 2u; ++source) {
        const gh_cv_complex_t *x = gh_cv_src[source];
        const gh_cv_complex_t *h = gh_cv_ir[1u - source];
        gh_cv_complex_t *y = gh_cv_src[source];
        for (i = 0u; i < GH_CV_N; ++i) {
            const float re = x[i].re * h[i].re - x[i].im * h[i].im;
            const float im = x[i].re * h[i].im + x[i].im * h[i].re;
            y[i].re = re;
            y[i].im = im;
        }
        ghost_cv_fft(y, 1);
        for (i = 0u; i < GH_CV_B; ++i)
            gh_cv_output[i] += 0.5f * y[GH_CV_B + i].re;
    }
}
static float ghost_convolution_sample(float u, float v)
{
    const float out = gh_cv_output[gh_cv_position];
    gh_cv_current[0][gh_cv_position] = u;
    gh_cv_current[1][gh_cv_position] = v;
    if (++gh_cv_position >= GH_CV_B) {
        gh_cv_position = 0u;
        ghost_cv_process();
    }
    return out;
}

/* Two driven, damped, mutually modulated delay loops. The feedback of
 * each loop depends on the OTHER loop's state, but does not inject that
 * state as an independent input. With either source silent from start,
 * the corresponding loop stays zero and the audible cross-term is zero.
 *
 * Feedback is contractive: |0.72 +/- 0.13| + 0.08 < 1, including the
 * short secondary tap. No self-sustained free-running oscillation exists
 * in the absence of both external drone excitations. */
static float ghost_feedback_sample(float u, float v)
{
    const uint32_t w = gh_fb_write;
    const float a = gh_fb_line[0][(w - 1207u) & GH_FB_MASK];
    const float b = gh_fb_line[1][(w - 1889u) & GH_FB_MASK];
    const float a2 = gh_fb_line[0][(w - 443u) & GH_FB_MASK];
    const float b2 = gh_fb_line[1][(w - 719u) & GH_FB_MASK];
    const float mod_a = 0.72f + 0.13f * fm_tanhf(2.5f * b);
    const float mod_b = 0.72f - 0.13f * fm_tanhf(2.5f * a);
    const float next_a = fm_tanhf(2.9f * u + mod_a * a + 0.08f * a2);
    const float next_b = fm_tanhf(2.9f * v + mod_b * b - 0.08f * b2);
    /* The output is a real interaction, not a duplicate of either drone:
     * every term vanishes when either loop is unexcited. */
    const float joint = 0.90f * a * b + 0.35f * a2 * b - 0.25f * a * b2;
    gh_fb_line[0][w] = next_a;
    gh_fb_line[1][w] = next_b;
    gh_fb_write = (w + 1u) & GH_FB_MASK;
    gh_fb_lp += 0.44f * (joint - gh_fb_lp);
    gh_fb_dc += 0.00024f * (gh_fb_lp - gh_fb_dc);
    return gh_fb_lp - gh_fb_dc;
}

static void ghost_batch3_reset(void)
{
    dr_memset(gh_cv_previous, 0, sizeof gh_cv_previous);
    dr_memset(gh_cv_current, 0, sizeof gh_cv_current);
    dr_memset(gh_cv_kernel, 0, sizeof gh_cv_kernel);
    dr_memset(gh_cv_output, 0, sizeof gh_cv_output);
    dr_memset(gh_cv_src, 0, sizeof gh_cv_src);
    dr_memset(gh_cv_ir, 0, sizeof gh_cv_ir);
    dr_memset(gh_fb_line, 0, sizeof gh_fb_line);
    gh_cv_position = gh_fb_write = 0u;
    gh_fb_lp = gh_fb_dc = 0.0f;
    gh_b3_power[0] = gh_b3_power[1] = 0.0f;
    gh_b3_gain[0] = gh_b3_gain[1] = 2.0f;
}

static void ghost_batch3_levels(float conv, float feedback)
{
    gh_b3_power[0] += 0.00020f * (conv * conv - gh_b3_power[0]);
    gh_b3_power[1] += 0.00020f * (feedback * feedback - gh_b3_power[1]);
}

static void ghost_batch3_gain_update(uint32_t n)
{
    const float parent = fm_sqrtf(ghost.power[0] + ghost.power[1]);
    const float slew = fm_minf((float)n / (0.42f * DR_SR), 1.0f);
    uint32_t i;
    for (i = 0u; i < 2u; ++i) {
        const float rms = fm_sqrtf(gh_b3_power[i] + 1e-7f);
        const float target = fm_clampf((i == 0u ? 0.58f : 0.52f) * parent / rms,
                                       0.14f, i == 0u ? 96.0f : 48.0f);
        gh_b3_gain[i] += slew * (target - gh_b3_gain[i]);
        gh_b3_power[i] = fm_flush(gh_b3_power[i]);
    }
    gh_fb_lp = fm_flush(gh_fb_lp);
    gh_fb_dc = fm_flush(gh_fb_dc);
}
static float ghost_batch3_level(float x, unsigned which)
{
    const float a = fm_clampf(x * gh_b3_gain[which], -1.0f, 1.0f);
    return a * (1.0f - a * a / 3.0f);
}
