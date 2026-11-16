/* SPDX-License-Identifier: GPL-3.0-only */
/* Audio-engine regression: each drone's STILL clock is independent, both can
 * freeze simultaneously, and a return matches the common master position. */
#include <stdio.h>
#include <math.h>
#include "drone.h"

static int32_t out[512];
static int fails;
static void blocks(int n)
{
    while (n-- > 0) dr_render(out, 256, 3277);
}
static void check(const char *name, int pass)
{
    printf("%-55s %s\n", name, pass ? "ok" : "FAIL");
    if (!pass) fails++;
}
static float pdist(float a, float b)
{
    float x = fabsf(a - b);
    return x < 0.5f ? x : 1.0f - x;
}
int main(void)
{
    float l0, l1, s0, s1;
    dr_init();
    dr_set(P_MORPH, 25);           /* approximately one second */
    dr_set(P_LFO_RATE, 87);        /* roughly thirteen seconds per cycle */
    dr_set(P_PULSE_RATE, 25);
    dr_set(P_PULSE, 1);
    blocks(8);

    /* Key 3: STILL I. Key 7: STILL II. */
    dr_subtract(2, 1000);
    blocks(8);
    l0 = dr_lfo_dr_ph[0]; s0 = dr_pulse_dr_ph[0];
    blocks(180);                   /* approximately 1.04 seconds */
    check("drone I LFO freezes", pdist(dr_lfo_dr_ph[0], l0) < 0.02f);
    check("drone II LFO continues", pdist(dr_lfo_dr_ph[1], l0) > 0.03f);
    check("drone I Swell freezes", pdist(dr_pulse_dr_ph[0]*0.5f, s0*0.5f) < 0.03f);

    dr_subtract(6, 1000);
    blocks(8);
    l1 = dr_lfo_dr_ph[1]; s1 = dr_pulse_dr_ph[1];
    blocks(160);
    check("drone II LFO freezes independently", pdist(dr_lfo_dr_ph[1], l1) < 0.02f);
    check("drone II Swell freezes independently",
          pdist(dr_pulse_dr_ph[1]*0.5f, s1*0.5f) < 0.03f);

    dr_still_rejoin(0, 1);
    dr_still_rejoin(1, 1);
    dr_subtract(2, 0);
    dr_subtract(6, 0);
    blocks(280);                   /* beyond the configured Morph Time */
    check("drone I LFO rejoins master", pdist(dr_lfo_dr_ph[0], dr_lfo_ph) < 0.02f);
    check("drone II LFO rejoins master", pdist(dr_lfo_dr_ph[1], dr_lfo_ph) < 0.02f);
    check("drone I Swell rejoins master",
          pdist(dr_pulse_dr_ph[0]*0.5f, dr_pulse_ph*0.5f) < 0.02f);
    check("drone II Swell rejoins master",
          pdist(dr_pulse_dr_ph[1]*0.5f, dr_pulse_ph*0.5f) < 0.02f);
    return fails ? 1 : 0;
}
