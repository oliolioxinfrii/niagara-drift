/* SPDX-License-Identifier: GPL-3.0-only */
/* Restored GHOST mode 5: spectral cross-synthesis and nonlinear body. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define DR_SR 44100.0f
#define dr_memset memset
#define OM_GHOST_BLEND_REFERENCE 1
#include "../../firmware/src/dsp/fastmath.h"
static struct { float power[2]; } ghost;
#include "../../firmware/src/dsp/ghost_mode14.c"
#include "../../firmware/src/dsp/ghost_mode5.c"
static int failed;
static void check(const char *label, int ok)
{
    printf("    %-48s %s\n", label, ok ? "ok" : "FAIL");
    failed += !ok;
}
int main(void)
{
    uint32_t i;
    double two = 0.0, alone = 0.0;
    int finite = 1;
    gh14_reset();
    gh14_tune(41.0f, 59.0f);
    gh5_prepare();
    gh5_reset();
    check("12-band coefficients initialized", gh5_bands[0][0].a1 > 0.0f &&
          gh5_bands[1][11].a2 > 0.0f);
    ghost.power[0] = 0.02f;
    ghost.power[1] = 0.03f;
    for (i = 0u; i < 44100u; ++i) {
        const float a = 0.18f * sinf((float)i * 0.017f);
        const float b = 0.19f * sinf((float)i * 0.031f);
        const float body = gh5_body_resonate(gh5_body_excitation(a, b));
        const float cross = gh5_cross_synthesis(a, b);
        const float y = gh5_mode5_output(body, cross);
        if (i > 22050u) two += (double)y * (double)y;
        if (!isfinite(y) || fm_fabsf(y) > 2.0f) finite = 0;
        if ((i & 255u) == 0u) gh5_finish(256u);
    }
    check("mode-5 two-parent output active and bounded", finite && two > 1e-10);
    gh5_reset();
    ghost.power[0] = 0.0f;
    ghost.power[1] = 0.03f;
    for (i = 0u; i < 44100u; ++i) {
        const float b = 0.18f * sinf((float)i * 0.022f);
        const float body = gh5_body_resonate(gh5_body_excitation(0.0f, b));
        const float cross = gh5_cross_synthesis(0.0f, b);
        const float y = gh5_mode5_output(body, cross);
        alone += (double)y * (double)y;
        if ((i & 255u) == 0u) gh5_finish(256u);
    }
    check("one parent alone cannot drive cross-synth/body", alone < 1e-12);
    gh5_reset();
    check("reset clears states while retaining filter coefficients",
          gh5_b_power == 0.0f && gh5_xs_lp == 0.0f &&
          gh5_bands[1][0].ic1 == 0.0f && gh5_bands[0][0].a1 > 0.0f);
    return failed ? 1 : 0;
}
