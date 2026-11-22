/* SPDX-License-Identifier: GPL-3.0-only */
/* Isolated Mode 14 unit tests with synthetic parent inputs. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define DR_SR 44100.0f
#define dr_memset memset
#include "../../firmware/src/dsp/fastmath.h"
static struct { float power[2]; } ghost;
#include "../../firmware/src/dsp/ghost_mode14.c"
static int fail_count;
static void check(const char *label, int ok)
{
    printf("    %-44s %s\n", label, ok ? "ok" : "FAIL");
    if (!ok) fail_count++;
}
int main(void)
{
    unsigned i;
    double two = 0.0, alone = 0.0;
    int finite = 1;
    gh14_reset();
    gh14_tune(41.0f, 59.0f);
    ghost.power[0] = 0.02f;
    ghost.power[1] = 0.03f;
    for (i = 0; i < 44100u; i++) {
        float a = 0.18f * sinf((float)i * 0.017f);
        float b = 0.19f * sinf((float)i * 0.031f);
        float y = gh14_output(gh14_excited(gh14_terrain(a, b)));
        if (i > 22050u) two += (double)y * y;
        if (!isfinite(y) || fm_fabsf(y) > 2.0f) finite = 0;
        if ((i & 255u) == 0u) gh14_finish(256u);
    }
    check("mode-14 two-parent output active and bounded", finite && two > 1e-10);
    gh14_reset();
    ghost.power[0] = 0.0f;
    ghost.power[1] = 0.03f;
    for (i = 0; i < 44100u; i++) {
        const float y = gh14_output(gh14_excited(gh14_terrain(0.0f,
                              0.18f * sinf((float)i * 0.022f))));
        alone += (double)y*y;
        if ((i & 255u) == 0u) gh14_finish(256u);
    }
    check("one parent alone cannot excite mode 14", alone < 1e-14);
    gh14_reset();
    check("mode-14 reset clears synthesis state",
          gh14_power == 0.0f && gh14_terrain_lp == 0.0f &&
          gh14_r[0][0] == 0.0f && gh14_gain == 1.0f);
    return fail_count ? 1 : 0;
}
