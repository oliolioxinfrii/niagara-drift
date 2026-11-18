/* SPDX-License-Identifier: GPL-3.0-only */
/* Self-contained mathematical checks for the two browser-only GHOST
 * Batch 3 algorithms. No complete synthesizer required. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define DR_SR 44100.0f
#define dr_memset memset
#include "../../firmware/src/dsp/fastmath.h"
static struct { float power[2]; } ghost;
#include "../../firmware/src/dsp/ghost_batch3.c"

static int errors;
static void ck(const char *name, int good)
{
    printf("    %-42s %s\n", name, good ? "ok" : "FAIL");
    if (!good) ++errors;
}
static float test_wave(float hz, int i)
{
    return 0.18f * sinf(FM_TWO_PI * hz * (float)i / DR_SR);
}
int main(void)
{
    int i;
    float maxerr = 0.0f, conv_rms = 0.0f, feedback_rms = 0.0f;
    float conv_one = 0.0f, fb_one = 0.0f, conv_tail = 0.0f, fb_tail = 0.0f;
    static gh_cv_complex_t roundtrip[GH_CV_N];

    /* An impulse displaced within the transform tests both bit reversal
     * and the forward/inverse phase sign, not merely the DC coefficient. */
    roundtrip[37].re = 1.0f;
    ghost_cv_fft(roundtrip, 0);
    ghost_cv_fft(roundtrip, 1);
    for (i = 0; i < (int)GH_CV_N; ++i) {
        const float correct = i == 37 ? 1.0f : 0.0f;
        const float e = fabsf(roundtrip[i].re - correct) +
                        fabsf(roundtrip[i].im);
        if (e > maxerr) maxerr = e;
    }
    printf("    convolution FFT roundtrip max error: %.7g\n", (double)maxerr);
    ck("FFT roundtrip of displaced impulse", maxerr < 0.002f);

    ghost_batch3_reset();
    /* Two distinct, non-coincident pitches should give a measurable
     * output from the LIVE convolution, not only under exact unison. */
    for (i = 0; i < 44100; ++i) {
        const float u = test_wave(110.0f, i), v = test_wave(176.0f, i);
        const float out = ghost_convolution_sample(u, v);
        if (i >= 22050) conv_rms += out * out;
    }
    conv_rms = sqrtf(conv_rms / 22050.0f);
    printf("    live convolution non-unison RMS: %.7g\n", (double)conv_rms);
    ck("moving convolution is non-silent and bounded",
       isfinite(conv_rms) && conv_rms > 0.000005f && conv_rms < 1.0f);

    ghost_batch3_reset();
    for (i = 0; i < 44100; ++i) {
        const float out = ghost_convolution_sample(test_wave(110.0f, i), 0.0f);
        conv_one += out * out;
    }
    ck("convolution requires two initial source signals", conv_one == 0.0f);

    ghost_batch3_reset();
    for (i = 0; i < 44100; ++i)
        (void)ghost_convolution_sample(test_wave(110.0f, i),
                                       test_wave(176.0f, i));
    for (i = 0; i < 88200; ++i) {
        const float out = ghost_convolution_sample(0.0f, 0.0f);
        if (i >= 44100) conv_tail += out * out;
    }
    ck("convolution's captured IR decays after silence",
       isfinite(conv_tail) && conv_tail < 1e-8f);

    ghost_batch3_reset();
    for (i = 0; i < 44100; ++i) {
        const float out = ghost_feedback_sample(test_wave(110.0f, i),
                                                test_wave(176.0f, i));
        if (i >= 22050) feedback_rms += out * out;
    }
    feedback_rms = sqrtf(feedback_rms / 22050.0f);
    printf("    coupled feedback non-unison RMS: %.7g\n", (double)feedback_rms);
    ck("coupled feedback is non-silent and bounded",
       isfinite(feedback_rms) && feedback_rms > 0.000005f && feedback_rms < 1.0f);

    ghost_batch3_reset();
    for (i = 0; i < 44100; ++i) {
        const float out = ghost_feedback_sample(test_wave(110.0f, i), 0.0f);
        fb_one += out * out;
    }
    ck("coupled feedback needs both source excitations", fb_one == 0.0f);

    ghost_batch3_reset();
    for (i = 0; i < 44100; ++i)
        (void)ghost_feedback_sample(test_wave(110.0f, i),
                                    test_wave(176.0f, i));
    for (i = 0; i < 88200; ++i) {
        const float out = ghost_feedback_sample(0.0f, 0.0f);
        if (i >= 44100) fb_tail += out * out;
    }
    printf("    coupled feedback silence-tail sum of squares: %.7g\n",
           (double)fb_tail);
    ck("coupled feedback is not self-sustaining",
       isfinite(fb_tail) && fb_tail < 1e-6f);

    if (errors)
        printf("ghost-batch3: %d FAILED\n", errors);
    else
        puts("ghost-batch3: all ok");
    return errors ? 1 : 0;
}
