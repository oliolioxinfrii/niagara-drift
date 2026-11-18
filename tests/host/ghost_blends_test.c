/* SPDX-License-Identifier: GPL-3.0-only */
/* Standalone checks for the fifteen-mode GHOST listening blend matrix.
 * Uses production blend code with synthetic normalized engine signals.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../firmware/src/dsp/drone.h"
#include "../../firmware/src/dsp/fastmath.h"
#define dr_memset memset
static struct { float power[2]; } ghost;
#define GH_BODY_MODES 8u
typedef struct { float re, im, c, s, pole, injection; } ghost_body_mode_t;
static ghost_body_mode_t gh_body[GH_BODY_MODES];
#include "../../firmware/src/dsp/ghost_blends.c"

static int failures;
static void verify(const char *label, int good)
{
    printf("    %-48s %s\n",label,good?"ok":"FAIL");
    if (!good) ++failures;
}
static float shaped(float x)
{
    const float a = fm_clampf(x,-1.0f,1.0f);
    return a*(1.0f-a*a/3.0f);
}
static int near(float a, float b)
{
    return fabsf(a-b)<0.00002f;
}
int main(void)
{
    float m[DR_NGHOSTMODES], max_exc = 0.0f, tail = 0.0f;
    unsigned i;
    verify("fifteen GHOST modes enumerated", DR_NGHOSTMODES == 15);
    ghost_blends_reset();
    ghost_blends_fill(m, 0.20f,0.30f,0.15f,0.40f,0.10f,0.35f);
    verify("five independent sources are unchanged",
           near(m[0],.20f)&&near(m[1],.30f)&&
           near(m[2],.15f)&&near(m[3],.40f)&&near(m[4],.10f));
    verify("Cross+Body 70/30",
           near(m[DR_GHOST_CROSS_BODY],shaped(.70f*.40f+.30f*.20f)));
    verify("Cross+Terrain 70/30",
           near(m[DR_GHOST_CROSS_TERRAIN],shaped(.70f*.40f+.30f*.30f)));
    verify("Cross+Feedback 70/30",
           near(m[DR_GHOST_CROSS_FEEDBACK],shaped(.70f*.40f+.30f*.10f)));
    verify("Terrain+Body 60/40",
           near(m[DR_GHOST_TERRAIN_BODY],shaped(.60f*.30f+.40f*.20f)));
    verify("Body+Feedback 65/35",
           near(m[DR_GHOST_BODY_FEEDBACK],shaped(.65f*.20f+.35f*.10f)));
    verify("Cross+Heterodyne 70/30",
           near(m[DR_GHOST_CROSS_HETERODYNE],shaped(.70f*.40f+.30f*.15f)));
    verify("Cross+Body+Feedback 55/30/15",
           near(m[DR_GHOST_CROSS_BODY_FEEDBACK],
                shaped(.55f*.40f+.30f*.20f+.15f*.10f)));
    verify("Cross+Terrain+Body 50/30/20",
           near(m[DR_GHOST_CROSS_TERRAIN_BODY],
                shaped(.50f*.40f+.30f*.30f+.20f*.20f)));
    for (i=0u;i<DR_NGHOSTMODES;++i)
        if (!isfinite(m[i])) ++failures;
    verify("frequency-split mode is a separate signal",
           isfinite(m[DR_GHOST_SPLIT_BODY_TERRAIN]) &&
           fabsf(m[DR_GHOST_SPLIT_BODY_TERRAIN]-m[DR_GHOST_TERRAIN_BODY]) > 0.001f);

    ghost_blends_reset();
    ghost_blends_fill(m,0,0,0,0,0,0);
    for (i=0u;i<DR_NGHOSTMODES;++i)
        if (m[i] != 0.0f) ++failures;
    verify("silent from reset with no source", m[DR_GHOST_TERRAIN_EXCITES_BODY] == 0.0f);

    /* Drive a separately tuned resonant object with terrain, then stop.
     * The resonators must be excited by terrain and later decay to zero. */
    ghost_blends_reset();
    for (i=0u;i<GH_BODY_MODES;++i) {
        const float turns = (55.0f+44.0f*(float)i)/DR_SR;
        gh_body[i].c = fm_cosf(FM_TWO_PI*turns);
        gh_body[i].s = fm_sin_turns(turns);
        gh_body[i].pole = 0.996f;
        gh_body[i].injection = 0.015f;
    }
    for (i=0u;i<10000u;++i) {
        const float y=ghost_terrain_excites_body_sample(0.07f);
        if (fm_fabsf(y)>max_exc) max_exc=fm_fabsf(y);
    }
    verify("terrain actually excites a separate resonant body",
           max_exc>0.00001f&&max_exc<2.0f);
    for (i=0u;i<44000u;++i)
        tail=ghost_terrain_excites_body_sample(0.0f);
    verify("terrain-driven body decays after excitation stops",
           isfinite(tail)&&fabsf(tail)<0.00001f);

    ghost.power[0]=ghost.power[1]=0.1f;
    for (i=0u;i<2000u;++i) {
        ghost_blends_fill(m,.20f,.30f,.15f,.40f,.10f,.35f);
        if ((i%256u)==0u) ghost_blends_gain_update(256u);
    }
    for (i=DR_GHOST_CROSS_BODY;i<DR_NGHOSTMODES;++i)
        if (!isfinite(gh_blend_gain[i]) ||
            gh_blend_gain[i]<0.48f || gh_blend_gain[i]>3.0f)
            ++failures;
    verify("blend levelling remains bounded", failures==0);
    if (failures) printf("ghost-blends: %d failed\n", failures);
    else puts("ghost-blends: all ok");
    return failures!=0;
}
