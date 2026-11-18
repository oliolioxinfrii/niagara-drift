/* SPDX-License-Identifier: GPL-3.0-only */
/* GHOST mode 14: Wave Terrain -> tuned eight-resonator body.
 * Its equations retain the listening experiment's ghost_batch1.c and
 * ghost_blends.c. In the hybrid morph, mode 5 can excite this same bank;
 * the A/B reference instead maintains an independent mode-5 body.
 * The parent drones, their post-filter inputs and shared FX are unchanged.
 */
#define GH14_N 8u
typedef struct {
    float re, im, c, s, pole, injection;
} gh14_res_t;
static gh14_res_t gh14_body[GH14_N];
static float gh14_r[GH14_N][2];
static float gh14_terrain_lp, gh14_terrain_dc;
static float gh14_power, gh14_gain = 1.0f;

static void gh14_reset(void)
{
    dr_memset(gh14_body, 0, sizeof gh14_body);
    dr_memset(gh14_r, 0, sizeof gh14_r);
    gh14_terrain_lp = gh14_terrain_dc = gh14_power = 0.0f;
    gh14_gain = 1.0f;
}
/* Per-block tuning. A future CPU1 job will receive copies of these pitches. */
static void gh14_tune(float pitch0, float pitch1)
{
    static const float tau[GH14_N] = {
        0.19f, 0.24f, 0.29f, 0.35f, 0.18f, 0.25f, 0.42f, 0.16f
    };
    float f[GH14_N];
    const float a = 440.0f * fm_exp2f((pitch0 - 69.0f) / 12.0f);
    const float b = 440.0f * fm_exp2f((pitch1 - 69.0f) / 12.0f);
    unsigned i;
    f[0] = fm_fabsf(a - b);
    f[1] = a + b;
    f[2] = fm_fabsf(2.0f*a - b);
    f[3] = fm_fabsf(2.0f*b - a);
    f[4] = 2.0f*a + b;
    f[5] = a + 2.0f*b;
    f[6] = fm_fabsf(3.0f*a - 2.0f*b);
    f[7] = 0.727f * (a + b);
    for (i = 0u; i < GH14_N; ++i) {
        const float hz = fm_clampf(f[i], 25.0f, 2900.0f);
        const float turns = hz / DR_SR;
        const float p = fm_exp2f(-1.44269504088896f / (tau[i] * DR_SR));
        gh14_body[i].c = fm_cosf(FM_TWO_PI * turns);
        gh14_body[i].s = fm_sin_turns(turns);
        gh14_body[i].pole = p;
        gh14_body[i].injection = 3.0f * (1.0f - p);
    }
}
/* Finite difference removes both one-parent terms. */
static float gh14_height(float x, float y, float bend)
{
    return 0.64f * fm_sinf(5.4f*x + 3.1f*y + 3.7f*x*y + bend*x*x)
         + 0.34f * fm_cosf(4.0f*x - 5.3f*y + 2.9f*x*y)
         + 0.24f * fm_sinf(6.0f*(x*x-y*y) + 3.6f*x*y);
}
static float gh14_terrain(float u, float v)
{
    const float x = fm_tanhf(2.8f*u), y = fm_tanhf(2.8f*v);
    const float p0 = ghost.power[0], p1 = ghost.power[1];
    const float bend = 1.1f + 0.75f * (p0-p1)/(p0+p1+0.005f);
    const float a = gh14_height(x,y,bend);
    const float b = gh14_height(x,0.0f,bend);
    const float c = gh14_height(0.0f,y,bend);
    const float d = 0.34f;
    const float cross = 0.11f * (a - b - c + d);
    gh14_terrain_lp += 0.48f * (cross - gh14_terrain_lp);
    gh14_terrain_dc += 0.00018f * (gh14_terrain_lp - gh14_terrain_dc);
    return gh14_terrain_lp - gh14_terrain_dc;
}
/* A single tuned resonator bank may receive either body excitation or
 * terrain excitation; mode 14's original wrapper retains its equations. */
static float gh14_resonate(float exc)
{
    float out = 0.0f;
    uint32_t i;
    for (i = 0u; i < GH14_N; ++i) {
        const gh14_res_t *tune = &gh14_body[i];
        float *m = gh14_r[i];
        const float re = m[0], im = m[1];
        m[0] = tune->pole * (tune->c * re - tune->s * im)
             + tune->injection * exc;
        m[1] = tune->pole * (tune->s * re + tune->c * im);
        out += (i < 4u ? 0.31f : 0.18f) * m[0];
    }
    return out;
}
static float gh14_excited(float raw_terrain)
{
    return gh14_resonate(0.82f * fm_tanhf(14.0f * raw_terrain));
}
static float gh14_output(float excited)
{
    const float x = fm_clampf(excited * gh14_gain, -1.0f, 1.0f);
    gh14_power += 0.00018f * (excited*excited - gh14_power);
    return x * (1.0f - x*x*(1.0f/3.0f));
}
static void gh14_finish(uint32_t n)
{
    const float parent = fm_sqrtf(ghost.power[0] + ghost.power[1]);
    const float slew = fm_minf((float)n/(0.65f*DR_SR), 1.0f);
    const float rms = fm_sqrtf(gh14_power + 1e-6f);
    const float desired = fm_clampf(0.53f * parent / rms, 0.48f, 3.0f);
    unsigned i;
    gh14_gain += slew * (desired - gh14_gain);
    gh14_power = fm_flush(gh14_power);
    gh14_terrain_lp = fm_flush(gh14_terrain_lp);
    gh14_terrain_dc = fm_flush(gh14_terrain_dc);
    for (i = 0u; i < GH14_N; ++i) {
        gh14_r[i][0] = fm_flush(gh14_r[i][0]);
        gh14_r[i][1] = fm_flush(gh14_r[i][1]);
    }
}
