/* SPDX-License-Identifier: GPL-3.0-only */
/* GHOST: Coupled Feedback (former Batch 3).
 * Live Convolution has been removed from the active firmware. This
 * module retains only the two source-driven, damped, nonlinear delay
 * networks. Included in drone.c as a single translation unit.
 */
#define GH_FB_RING 4096u
#define GH_FB_MASK (GH_FB_RING - 1u)

static float gh_fb_line[2][GH_FB_RING];
static uint32_t gh_fb_write;
static float gh_fb_lp, gh_fb_dc;
static float gh_fb_power, gh_fb_gain;

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
    dr_memset(gh_fb_line, 0, sizeof gh_fb_line);
    gh_fb_write = 0u;
    gh_fb_lp = gh_fb_dc = 0.0f;
    gh_fb_power = 0.0f;
    gh_fb_gain = 2.0f;
}
static void ghost_batch3_levels(float feedback)
{
    gh_fb_power += 0.00020f * (feedback*feedback - gh_fb_power);
}
static void ghost_batch3_gain_update(uint32_t n)
{
    const float parent = fm_sqrtf(ghost.power[0] + ghost.power[1]);
    const float slew = fm_minf((float)n / (0.42f * DR_SR), 1.0f);
    const float rms = fm_sqrtf(gh_fb_power + 1e-7f);
    const float target = fm_clampf(0.52f * parent / rms, 0.14f, 48.0f);
    gh_fb_gain += slew * (target - gh_fb_gain);
    gh_fb_power = fm_flush(gh_fb_power);
    gh_fb_lp = fm_flush(gh_fb_lp);
    gh_fb_dc = fm_flush(gh_fb_dc);
}
static float ghost_batch3_level(float x)
{
    const float a = fm_clampf(x * gh_fb_gain, -1.0f, 1.0f);
    return a * (1.0f - a*a/3.0f);
}
