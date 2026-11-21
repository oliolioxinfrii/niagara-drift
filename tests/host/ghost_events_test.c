/* SPDX-License-Identifier: GPL-3.0-only */
/* Source-driven GHOST event regression (the DSP internals are static in
 * the firmware unity build). The UI/latch behavior is separately covered
 * by tests/scenarios/ghost-excitation-keys.omni. */
#include "drone.c"
#include <math.h>
#include <stdio.h>

static int failed;

static void check_event(const char *name, int ok)
{
    printf("    %-59s %s\n", name, ok ? "ok" : "FAIL");
    failed += !ok;
}

/* Measure the actual modal INJECTION, not a changed parent instrument
 * waveform or FX output. 'paired' means both sources are sounding;
 * 'single' removes the first source while the second remains audible. */
static float inject_rms(int event, int paired, int region)
{
    double sum = 0.0;
    int n;
    dr_init();
    ghost_event_reset();
    ghost.power[0] = paired ? 0.023f : 0.0f;
    ghost.power[1] = 0.028f;
    ghost_event_fire((uint8_t)(1u << event));
    for (n = 0; n < 35280; ++n) {  /* 800ms, includes full TREMOR train */
        const float u = paired ? 0.17f * sinf((float)n * 0.021f) : 0.0f;
        const float v = 0.19f * sinf((float)n * 0.027f);
        float common = 0.0f, low = 0.0f, high = 0.0f;
        float value;
        ghost_event_tick(&common, &low, &high, u, v);
        value = region == 0 ? common : region == 1 ? low : high;
        if (!isfinite(value)) return -1.0f;
        sum += (double)value * (double)value;
    }
    return (float)sqrt(sum / 35280.0);
}

int main(void)
{
    const float scrape_live = inject_rms(1, 1, 2);
    const float scrape_one = inject_rms(1, 0, 2);
    const float bow_live = inject_rms(2, 1, 0);
    const float bow_one = inject_rms(2, 0, 0);
    const float strike = inject_rms(0, 1, 0);
    const float tremor = inject_rms(3, 1, 1);
    const float tremor_high = inject_rms(3, 1, 2);
    printf("    input coupling RMS: STRIKE %.3f, SCRAPE %.3f / one-parent %.7f, "
           "BOW %.3f / one-parent %.7f, TREMOR low %.3f high %.3f\n",
           (double)strike, (double)scrape_live, (double)scrape_one,
           (double)bow_live, (double)bow_one, (double)tremor,
           (double)tremor_high);
    check_event("SCRAPE is sourced from the two parent waves",
                scrape_live > 0.1f && scrape_one < 0.000001f);
    check_event("BOW follows the parents and cannot generate standalone noise",
                bow_live > 0.05f && bow_one < 0.000001f);
    check_event("STRIKE is a stronger, finite impact",
                strike > 0.1f && strike < 6.0f);
    check_event("TREMOR has a distinct low-mode pulse train",
                tremor > 0.1f && tremor > 8.0f * tremor_high &&
                tremor < 6.0f);
    printf(failed ? "ghost events: %d FAILED\n" : "ghost events: all ok\n", failed);
    return failed ? 1 : 0;
}
