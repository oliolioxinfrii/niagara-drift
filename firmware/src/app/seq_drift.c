/* SPDX-License-Identifier: GPL-3.0-only */
/* Experimental four-position pitch progression for the two existing drones.
 * The sequence edits notes, not synthesis voices or scene states. RAM only.
 * Step 1 always follows the live pitches of HOME > Pitch (the second HOME
 * page), even when the sequencer is running. */
#define SD_MAX 4
#define SD_LIMIT_S 240u
#define SD_DEFAULT_S 10u
static struct {
    uint8_t count, position, drone, phase, enabled, paused;
    uint16_t gap_s[SD_MAX];              /* adjacent intervals; final one wraps to step 1 */
    int16_t note[2][SD_MAX];
    int16_t sent[2];
    uint32_t started_ms, sent_ms, paused_ms;
    int8_t preview_key;                  /* most recently pressed keyboard key; -1: none */
    uint8_t preview_sounded;             /* audition temporarily started PLAY's audio */
    uint8_t preview_active;              /* audible preview replaced the normal pitch */
    uint8_t lock_was;
    int16_t lock_offset;                  /* Pitch Lock without Pitch Link: retain initial interval */
} sd;

static uint32_t sd_total_s(void)
{
    uint32_t sum = 0;
    int i;
    if (sd.count < 2) return 0;
    for (i = 0; i < sd.count; i++) sum += sd.gap_s[i];
    return sum;
}
static uint32_t sd_node_s(int pos)       /* elapsed seconds from step 1 */
{
    uint32_t sum = 0;
    int i;
    for (i = 0; i < pos && i < sd.count; i++) sum += sd.gap_s[i];
    return sum;
}
static int sd_linked(void) { return proj.par[P_PLINK] != 0 || proj.par[P_PLOCK] != 0; }
static int sd_offset(void)
{
    return proj.par[P_PLINK] ? proj.par[P_PLINK_OFF] : sd.lock_offset;
}
static int sd_pitch(int g, int pos)
{
    if (g && sd_linked())
        return sd.note[0][pos] + sd_offset();
    return sd.note[g][pos];
}

static uint32_t sd_elapsed_ms(void)
{
    return sd.paused ? sd.paused_ms : plat_ms() - sd.started_ms;
}
static void sd_reset_clock(void)
{
    sd.started_ms = plat_ms();
    sd.paused_ms = 0;
    sd.paused = 0;
    sd.sent_ms = 0;
    sd.sent[0] = sd.sent[1] = -32768;
}
static void sd_sync_home(void)
{
    int link, offset, i;
    if (proj.par[P_PLOCK] && !sd.lock_was)
        sd.lock_offset = (int16_t)(proj.par[DR_GP(1, GP_PITCH)] - proj.par[DR_GP(0, GP_PITCH)]);
    sd.lock_was = (uint8_t)(proj.par[P_PLOCK] != 0);
    link = sd_linked();
    offset = sd_offset();
    sd.note[0][0] = proj.par[DR_GP(0, GP_PITCH)];
    sd.note[1][0] = link ? (int16_t)(sd.note[0][0] + offset) : proj.par[DR_GP(1, GP_PITCH)];
    if (link)
        for (i = 1; i < SD_MAX; i++)
            sd.note[1][i] = (int16_t)(sd.note[0][i] + offset);
}
static void sd_init(void)
{
    int i, g;
    memset(&sd, 0, sizeof sd);
    sd.count = 1;
    sd.preview_key = -1;
    for (i = 0; i < SD_MAX; i++) {
        sd.gap_s[i] = SD_DEFAULT_S;
        for (g = 0; g < 2; g++)
            sd.note[g][i] = proj.par[DR_GP(g, GP_PITCH)];
    }
    sd_sync_home();
    sd_reset_clock();
}
static void sd_restore_pitch(void)
{
    int g;
    for (g = 0; g < 2; g++) {
        dr_slide(g, 80);
        dr_set(DR_GP(g, GP_PITCH), proj.par[DR_GP(g, GP_PITCH)]);
    }
    dr_set(P_PLINK_OFF, proj.par[P_PLINK_OFF]);
    dr_set(P_PLINK, proj.par[P_PLINK]);
}
static void sd_stop(void)
{
    sd.enabled = 0;
    sd_reset_clock();
    if (sd.preview_key < 0) sd_restore_pitch();
}
static int sd_at(int g, uint32_t elapsed_ms);
static void sd_toggle(void)
{
    if (!sd.enabled) {
        sd_sync_home();
        sd.enabled = 1;
        sd_reset_clock();
        /* Pitch Link applies to the authored notes; the audio engine follows
         * the sequence's two independently calculated cursor pitches. */
        dr_set(P_PLINK, 0);
        say_c("SEQUENCE", "PLAY", K_GOLD);
    } else if (!sd.paused) {
        int g;
        sd.paused_ms = sd_elapsed_ms();
        sd.paused = 1;
        /* Freeze at the exact interpolation point, including the phase offset.
         * The sound stays on; ordinary PLAY is the stop/reset control. */
        for (g = 0; g < 2; g++) {
            dr_slide(g, 25);
            dr_set(DR_GP(g, GP_PITCH), sd_at(g, sd.paused_ms));
        }
        sd.sent[0] = sd.sent[1] = -32768;
        say_c("SEQUENCE", "PAUSED", K_GOLD);
    } else {
        sd.started_ms = plat_ms() - sd.paused_ms;
        sd.paused = 0;
        sd.sent_ms = 0;
        sd.sent[0] = sd.sent[1] = -32768;
        say_c("SEQUENCE", "RESUME", K_GOLD);
    }
}

/* Smoothly interpolate across unequal event intervals. In Pitch Link mode,
 * drone II follows drone I at the saved offset, with no phase disagreement. */
static int sd_at(int g, uint32_t elapsed_ms)
{
    const uint32_t len_ms = sd_total_s() * 1000u;
    uint32_t t;
    int i, base = g && sd_linked() ? 0 : g;
    if (!len_ms) return sd_pitch(g, 0);
    t = elapsed_ms % len_ms;
    if (g && !sd_linked())
        t = (t + (uint32_t)sd.phase * len_ms / 100u) % len_ms;
    for (i = 0; i < sd.count; i++) {
        const uint32_t gap = (uint32_t)sd.gap_s[i] * 1000u;
        if (t < gap) {
            const int a = sd.note[base][i];
            const int b = sd.note[base][(i + 1) % sd.count];
            const float x = (float)t / (float)gap;
            const float smooth = x * x * (3.0f - 2.0f * x);
            const float v = (float)a + (float)(b - a) * smooth +
                            (g && sd_linked() ? (float)sd_offset() : 0.0f);
            return (int)(v + (v >= 0.0f ? 0.5f : -0.5f));
        }
        t -= gap;
    }
    return sd_pitch(g, 0);
}
static void sd_note_set(int pos, int g, int cents)
{
    if (sd_linked()) g = 0;
    const int p = DR_GP(g, GP_PITCH);
    const dr_param_t *in = dr_param_info(p);
    if (cents < in->lo) cents = in->lo;
    if (cents > in->hi) cents = in->hi;
    if (pos == 0) {
        /* Home Pitch 2 is the authoritative source for the first note,
         * including edits made from the SEQ keyboard or SEL+K1. */
        param_set(DR_GP(g, GP_PITCH), cents);
        sd_sync_home();
    } else {
        sd.note[g][pos] = (int16_t)cents;
        if (sd_linked())
            sd.note[1][pos] = (int16_t)(cents + sd_offset());
    }
}
static int sd_key_cents(int key)
{
    /* The physical 27-key keyboard is F3 (MIDI 53) through G5 (MIDI 79).
     * Snap to the active tuning; SEL+K1 accesses intervals between keys. */
    const int p = DR_GP(sd_linked() ? 0 : sd.drone, GP_PITCH);
    return grid_near(p, (53 + key) * 100);
}
static void sd_keys(uint32_t down, uint32_t pressed)
{
    int i, newest = -1;
    for (i = 0; i < NKEYS; i++)
        if (pressed >> i & 1u) {
            newest = i;
            sd_note_set(sd.position, sd.drone, sd_key_cents(i));
        }
    if (newest >= 0) {
        sd.preview_key = (int8_t)newest;
        sd.sent[0] = sd.sent[1] = -32768;
    } else if (sd.preview_key >= 0 && !(down >> sd.preview_key & 1u)) {
        sd.preview_key = -1;
        sd.sent[0] = sd.sent[1] = -32768;
    }
}
/* Persistent LEDs indicate every programmed note currently on the keyboard.
 * The selected position is full brightness, other positions a dim glow. */
static void sd_key_leds(uint8_t *k)
{
    int i, g;
    memset(k, 0, NKEYS);
    for (i = 0; i < sd.count; i++)
        for (g = 0; g < 2; g++) {
            const int cents = sd_pitch(g, i);
            int key = (cents + 50) / 100 - 53;
            /* The keyboard spans only F3..G5; octave-fold off-panel
             * fundamentals so their pitch classes remain indicated. */
            while (key < 0) key += 12;
            while (key >= NKEYS) key -= 12;
            {
                const uint8_t level = i == sd.position && (!g || !sd_linked()) ? 255 : 90;
                if (k[key] < level) k[key] = level;
            }
        }
    if (sd.preview_key >= 0)
        k[(int)sd.preview_key] = 255;
}
static void sd_run(void)
{
    uint32_t now = plat_ms();
    int g;
    sd_sync_home();
    /* When any note is held, audition the exact programmed pitch, including
     * the linked drone. The normal progression resumes on release. */
    if (sd.preview_key >= 0) {
        const int audible_g = sd_linked() ? 0 : sd.drone;
        const int v = sd_pitch(audible_g, sd.position);
        if (!ui.playing && !sd.preview_sounded) {
            dr_play(1);
            sd.preview_sounded = 1;
        }
        if (!sd.preview_active || sd.sent[0] != v) {
            dr_set(P_PLINK, 0);
            for (g = 0; g < 2; g++) {
                const int pitch = sd_linked() ? sd_pitch(g, sd.position) :
                                  g == audible_g ? v : proj.par[DR_GP(g, GP_PITCH)];
                dr_slide(g, 30);
                dr_set(DR_GP(g, GP_PITCH), pitch);
            }
            sd.sent[0] = (int16_t)v;
        }
        sd.preview_active = 1;
        return;
    }
    if (sd.preview_active) {
        sd.preview_active = 0;
        sd.sent[0] = sd.sent[1] = -32768;
        sd.sent_ms = 0;
        if (sd.preview_sounded && !ui.playing) {
            dr_play(0);
            sd.preview_sounded = 0;
        }
        if (!sd.enabled || !ui.playing)
            sd_restore_pitch();
    }
    if (!sd.enabled || !ui.playing)
        return;
    if (sd.paused && sd.sent[0] != -32768 && sd.sent[1] != -32768)
        return;
    if (sd.sent_ms && now - sd.sent_ms < 50u) return;
    sd.sent_ms = now | 1u;
    dr_set(P_PLINK, 0);               /* scene recalls cannot re-enable link on the audio cursor */
    for (g = 0; g < 2; g++) {
        const int v = sd_at(g, sd_elapsed_ms());
        if (v != sd.sent[g]) {
            dr_slide(g, sd.paused ? 25 : 80);
            dr_set(DR_GP(g, GP_PITCH), v);
            sd.sent[g] = (int16_t)v;
        }
    }
}

/* Sequence page: K1 selects position, K2 selects drone, K3 event count,
 * K4 drone-II phase. SEL+K1 retunes the chosen position on its tuning grid.
 * Pitch Link makes drone II follow I and disables independent selection. */
static void sd_note_knob(int i, int32_t e)
{
    int v;
    if (i == 0 && (ui.btn >> B_SEL & 1u)) {
        const int g = sd_linked() ? 0 : sd.drone;
        const int p = DR_GP(g, GP_PITCH);
        const int old = sd_pitch(g, sd.position);
        const int next = grid_at(p, grid_k(p, old) + (e > 0 ? 1 : -1));
        sd_note_set(sd.position, g, next);
        ui.sel_turned = 1;
        ui.btn_used |= 1u << B_SEL;
    } else if (i == 0) {
        v = (int)sd.position + (e > 0 ? 1 : -1);
        sd.position = (uint8_t)((v + sd.count) % sd.count);
    } else if (i == 1) {
        if (sd_linked()) {
            sd.drone = 0;
            say_c("PITCHES LINKED", "II FOLLOWS I", K_DRONE[1]);
        } else {
            sd.drone = (uint8_t)(e > 0);
        }
    } else if (i == 2) {
        const int old = sd.count;
        v = (int)sd.count + (e > 0 ? 1 : -1);
        sd.count = (uint8_t)(v < 1 ? 1 : v > SD_MAX ? SD_MAX : v);
        if (sd.position >= sd.count) sd.position = sd.count - 1;
        if (sd.count > old) {
            const int step = sd.count - 1;
            sd.note[0][step] = sd.note[0][step - 1];
            sd.note[1][step] = sd.note[1][step - 1];
        }
        while (sd_total_s() > SD_LIMIT_S) {
            int largest = 0, j;
            for (j = 1; j < sd.count; j++)
                if (sd.gap_s[j] > sd.gap_s[largest]) largest = j;
            if (sd.gap_s[largest] < 2) break;
            sd.gap_s[largest]--;
        }
        /* Changing the step count changes the loop map, not its clock. */
        sd.sent[0] = sd.sent[1] = -32768;
    } else if (i == 3 && !sd_linked()) {
        v = (int)sd.phase + (int)accel(EN_K1 + i, e, 100);
        sd.phase = (uint8_t)(v < 0 ? 0 : v > 100 ? 100 : v);
    }
}

/* K4 rescales every interval, preserving event positions as fractions of
 * the complete loop. Integer-second boundaries stay ordered and at least one
 * second apart. The phase of a running or paused cursor also stays fixed. */
static void sd_scale_total(uint32_t new_total)
{
    const uint32_t old_total = sd_total_s();
    const uint32_t elapsed = sd_elapsed_ms();
    uint32_t at[SD_MAX + 1], prev = 0, running_ms;
    int j;
    if (new_total == old_total || old_total == 0) return;
    for (j = 1; j < sd.count; j++) {
        uint32_t t = (uint32_t)(((uint64_t)sd_node_s(j) * new_total + old_total / 2u) / old_total);
        if (t <= prev) t = prev + 1u;
        if (t > new_total - (uint32_t)(sd.count - j))
            t = new_total - (uint32_t)(sd.count - j);
        at[j] = t;
        prev = t;
    }
    at[0] = 0;
    at[sd.count] = new_total;
    for (j = 0; j < sd.count; j++)
        sd.gap_s[j] = (uint16_t)(at[j + 1] - at[j]);
    running_ms = (uint32_t)((uint64_t)(elapsed % (old_total * 1000u)) * new_total / old_total) +
                 (elapsed / (old_total * 1000u)) * new_total * 1000u;
    if (sd.paused)
        sd.paused_ms = running_ms;
    else if (sd.enabled)
        sd.started_ms = plat_ms() - running_ms;
    sd.sent[0] = sd.sent[1] = -32768;
    sd.sent_ms = 0;
}

/* K1-K3 move node 2/3/4 without moving their neighbors or the loop end.
 * K4 changes the duration of the whole loop without moving the normalized
 * event positions; the time ruler stretches to match the new duration. */
static void sd_time_knob(int i, int32_t e)
{
    int delta;
    if (sd.count < 2) return;
    if (i < 3) {
        if (i + 1 >= sd.count) return;
        delta = (int)accel(EN_K1 + i, e, SD_LIMIT_S);
        if (delta < 1 - (int)sd.gap_s[i]) delta = 1 - (int)sd.gap_s[i];
        if (delta > (int)sd.gap_s[i + 1] - 1) delta = (int)sd.gap_s[i + 1] - 1;
        sd.gap_s[i] = (uint16_t)((int)sd.gap_s[i] + delta);
        sd.gap_s[i + 1] = (uint16_t)((int)sd.gap_s[i + 1] - delta);
    } else {
        const uint32_t total = sd_total_s();
        int target = (int)total + (int)accel(EN_K1 + i, e, SD_LIMIT_S);
        if (target < sd.count) target = sd.count;
        if (target > (int)SD_LIMIT_S) target = (int)SD_LIMIT_S;
        sd_scale_total((uint32_t)target);
    }
}
