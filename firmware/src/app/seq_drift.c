/* SPDX-License-Identifier: GPL-3.0-only */
/* Experimental four-event pitch path for Niagara Drift.
 * Included by ui.c after the tuning and encoder helpers. No additional DSP voices:
 * interpolate the existing two drone fundamentals on a shared, unequal timeline.
 * These values are intentionally RAM-only while interaction is being evaluated. */
#define SD_MAX 4
#define SD_LIMIT_S 240u
#define SD_DEFAULT_S 10u
static struct {
    uint8_t count, position, drone, phase, enabled, running;
    uint16_t gap_s[SD_MAX];
    int16_t note[2][SD_MAX];
    uint32_t started_ms, sent_ms;
    int16_t sent[2];
} sd;

static uint32_t sd_total_s(void)
{
    uint32_t sum = 0;
    int i;
    if (sd.count < 2)
        return 0;
    for (i = 0; i < sd.count; i++)
        sum += sd.gap_s[i];
    return sum;
}

static void sd_reset_clock(void)
{
    sd.started_ms = plat_ms();
    sd.sent_ms = 0;
    sd.sent[0] = sd.sent[1] = -32768;
}

static void sd_init(void)
{
    int g, i;
    memset(&sd, 0, sizeof sd);
    sd.count = 1;
    for (i = 0; i < SD_MAX; i++) {
        sd.gap_s[i] = SD_DEFAULT_S;
        for (g = 0; g < 2; g++)
            sd.note[g][i] = proj.par[DR_GP(g, GP_PITCH)];
    }
    sd_reset_clock();
}

/* The sequencer owns pitch while enabled. Disabling restores the live project's
 * stored pitch and Pitch Link, without saving transient sequencer positions. */
static void sd_toggle(void)
{
    int g;
    sd.enabled = !sd.enabled;
    sd.running = 0;
    sd_reset_clock();
    if (sd.enabled) {
        dr_set(P_PLINK, 0);               /* independent phase and notes for both drones */
        say_c("SEQUENCE", "ON", K_GOLD);
    } else {
        for (g = 0; g < 2; g++) {
            dr_slide(g, 100);
            dr_set(DR_GP(g, GP_PITCH), proj.par[DR_GP(g, GP_PITCH)]);
        }
        dr_set(P_PLINK_OFF, proj.par[P_PLINK_OFF]);
        dr_set(P_PLINK, proj.par[P_PLINK]);
        say_c("SEQUENCE", "OFF", K_GOLD);
    }
}

/* Which two points bracket this drone's position? Return interpolated cents.
 * A smoothstep connects nodes without the abrupt change in slope of linear
 * interpolation. Drone II is offset over the COMPLETE cycle (like Swell). */
static int sd_at(int g, uint32_t elapsed_ms)
{
    const uint32_t len_ms = sd_total_s() * 1000u;
    uint32_t t;
    int i;
    if (!len_ms)
        return sd.note[g][0];
    t = elapsed_ms % len_ms;
    if (g)
        t = (t + (uint32_t)sd.phase * len_ms / 100u) % len_ms;
    for (i = 0; i < sd.count; i++) {
        const uint32_t gap = (uint32_t)sd.gap_s[i] * 1000u;
        if (t < gap) {
            const int a = sd.note[g][i], b = sd.note[g][(i + 1) % sd.count];
            const float x = (float)t / (float)gap;
            const float eased = x * x * (3.0f - 2.0f * x);
            const float c = (float)a + (float)(b - a) * eased;
            return (int)(c + (c < 0.0f ? -0.5f : 0.5f));
        }
        t -= gap;
    }
    return sd.note[g][0];
}

static void sd_run(void)
{
    uint32_t now;
    int g;
    if (!sd.enabled || !ui.playing) {
        sd.running = 0;
        return;
    }
    if (!sd.running) {
        sd.running = 1;
        sd_reset_clock();                /* start at step 1 on PLAY */
    }
    now = plat_ms();
    if (sd.sent_ms && now - sd.sent_ms < 50u)
        return;
    sd.sent_ms = now | 1u;
    for (g = 0; g < 2; g++) {
        const int v = sd_at(g, now - sd.started_ms);
        if (v != sd.sent[g]) {
            dr_slide(g, 80);              /* bridge the 50-ms control-rate updates */
            dr_set(DR_GP(g, GP_PITCH), v);
            sd.sent[g] = (int16_t)v;
        }
    }
}

/* K1 step, K2 drone (SEL+K2 tunes the selected step), K3 event count,
 * K4 phase of drone II, 0..100% of the complete sequence. */
static void sd_note_knob(int i, int32_t e)
{
    int v;
    if (i == 0) {
        v = (int)sd.position + (e > 0 ? 1 : -1);
        sd.position = (uint8_t)((v + sd.count) % sd.count);
    } else if (i == 1 && (ui.btn >> B_SEL & 1u)) {
        const int p = DR_GP(sd.drone, GP_PITCH);
        const dr_param_t *in = dr_param_info(p);
        const int old = sd.note[sd.drone][sd.position];
        v = grid_at(p, grid_k(p, old) + (e > 0 ? 1 : -1));
        v = v < in->lo ? in->lo : v > in->hi ? in->hi : v;
        sd.note[sd.drone][sd.position] = (int16_t)v;
        ui.sel_turned = 1;
        ui.btn_used |= 1u << B_SEL;
    } else if (i == 1) {
        sd.drone = (uint8_t)(e > 0);
    } else if (i == 2) {
        v = (int)sd.count + (e > 0 ? 1 : -1);
        sd.count = (uint8_t)(v < 1 ? 1 : v > SD_MAX ? SD_MAX : v);
        if (sd.position >= sd.count)
            sd.position = sd.count - 1;
        sd_reset_clock();
    } else if (i == 3) {
        v = (int)sd.phase + (int)accel(EN_K1 + i, e, 100);
        sd.phase = (uint8_t)(v < 0 ? 0 : v > 100 ? 100 : v);
    }
}

/* K1..4 give time from step i to i+1 (wrapping to step 1).
 * Each is 1s minimum. The sum, not a per-knob maximum, is capped. */
static void sd_time_knob(int i, int32_t e)
{
    uint32_t sum, maximum;
    int v;
    if (i >= sd.count || sd.count < 2)
        return;
    sum = sd_total_s();
    maximum = SD_LIMIT_S - (sum - sd.gap_s[i]);
    v = (int)sd.gap_s[i] + (int)accel(EN_K1 + i, e, SD_LIMIT_S);
    if (v < 1)
        v = 1;
    if ((uint32_t)v > maximum)
        v = (int)maximum;
    sd.gap_s[i] = (uint16_t)v;
}
