/* SPDX-License-Identifier: GPL-3.0-only */
/* Dedicated checks for the retained GHOST coupled-feedback DSP.
 * The live-convolution FFT and test were intentionally retired. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define DR_SR 44100.0f
#define dr_memset memset
#include "../../firmware/src/dsp/fastmath.h"
static struct { float power[2]; } ghost;
#include "../../firmware/src/dsp/ghost_batch3.c"

static int failed;
static void check(const char *name, int passed)
{
    printf("    %-48s %s\n", name, passed ? "ok" : "FAIL");
    if (!passed) ++failed;
}
static float wave(float hz, int n)
{
    return 0.18f * sinf(FM_TWO_PI * hz * (float)n / DR_SR);
}
int main(void)
{
    int i;
    double active = 0.0, lone = 0.0, tail = 0.0;
    float maxabs = 0.0f;

    ghost_batch3_reset();
    for (i = 0; i < 44100; ++i) {
        const float y = ghost_feedback_sample(wave(110.0f, i),
                                              wave(176.0f, i));
        if (i >= 22050) active += (double)y * y;
        if (fm_fabsf(y) > maxabs) maxabs = fm_fabsf(y);
        if (!isfinite(y)) { ++failed; break; }
    }
    active = sqrt(active / 22050.0);
    printf("    feedback steady RMS %.7g, max %.7g\n", active, (double)maxabs);
    check("two-source feedback is audible and finite",
          active > 0.000005 && active < 1.0 && maxabs < 2.0f);

    ghost_batch3_reset();
    for (i = 0; i < 44100; ++i) {
        const float y = ghost_feedback_sample(wave(110.0f, i), 0.0f);
        lone += (double)y * y;
    }
    check("feedback needs both sources from reset", lone == 0.0);

    ghost_batch3_reset();
    for (i = 0; i < 44100; ++i)
        (void)ghost_feedback_sample(wave(110.0f, i), wave(176.0f, i));
    for (i = 0; i < 88200; ++i) {
        const float y = ghost_feedback_sample(0.0f, 0.0f);
        if (i >= 44100) tail += (double)y * y;
    }
    printf("    feedback silence-tail power %.7g\n", tail);
    check("feedback decays after all excitation ceases",
          isfinite(tail) && tail < 1e-6);

    ghost_batch3_reset();
    ghost.power[0] = 0.1f;
    ghost.power[1] = 0.1f;
    for (i = 0; i < 100; ++i) {
        ghost_batch3_levels(0.02f);
        ghost_batch3_gain_update(256u);
    }
    check("independent feedback calibration stays finite",
          isfinite(gh_fb_gain) && gh_fb_gain > 0.14f &&
          gh_fb_gain <= 48.0f);

    if (failed) printf("ghost-feedback: %d FAILED\n", failed);
    else puts("ghost-feedback: all ok");
    return failed != 0;
}
