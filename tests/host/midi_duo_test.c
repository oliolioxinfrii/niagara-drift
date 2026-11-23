/* SPDX-License-Identifier: GPL-3.0-only */
/* Pure host allocator: only TWO drone slots, one optional third-note
 * pitch target, and CC64 per-channel sustain with physical-up tracking. */
#include <stdio.h>
#include "midi_duo.h"
static int fail;
static void check(const char *s, int ok)
{
    printf("    %-63s %s\n", s, ok ? "ok" : "FAIL");
    fail += !ok;
}
int main(void)
{
    midi_duo_t d;
    midi_duo_reset(&d);
    check("C2 first claims drone I",
          midi_duo_on(&d, 0, 36, 45.0f, 57.0f) == 0);
    check("C3 second claims drone II",
          midi_duo_on(&d, 0, 48, 36.0f, 57.0f) == 1);
    check("Duplicate held key cannot make a third voice",
          midi_duo_on(&d, 0, 48, 36.0f, 48.0f) == -1 &&
          d.extra == -1);
    check("B3 as third chooses nearest Drone II (48 to 59)",
          midi_duo_on(&d, 0, 59, 36.0f, 48.0f) == 3 &&
          d.extra_slot == 1 && midi_duo_slot_note(&d, 1) == 59 &&
          d.key[1] == 48);
    check("Original C3 released mid-glide; B3 continues uninterrupted",
          midi_duo_off(&d, 0, 48) == 1 &&
          midi_duo_slot_note(&d, 1) == 59 && d.extra_slot == 1);
    check("Fourth key does not steal B3 or create another voice",
          midi_duo_on(&d, 0, 70, 36.0f, 54.0f) == -1 &&
          d.extra == 59 && d.key[0] == 36);
    check("Release B3 after C3: glide ends and drone II is free",
          midi_duo_off(&d, 0, 59) == 3 &&
          midi_duo_slot_note(&d, 1) == -1);
    check("New B2 reuses exactly freed II, not still-held I",
          midi_duo_on(&d, 0, 47, 36.0f, 59.0f) == 1 &&
          d.key[0] == 36 && d.key[1] == 47);

    midi_duo_reset(&d);
    midi_duo_on(&d, 0, 36, 36.0f, 48.0f);
    midi_duo_on(&d, 0, 48, 36.0f, 48.0f);
    check("B2 third chooses nearest drone II (47 is one semitone below 48)",
          midi_duo_on(&d, 0, 47, 36.0f, 48.0f) == 3 &&
          d.extra_slot == 1);
    check("Releasing third before original returns to original pitch",
          midi_duo_off(&d, 0, 47) == 3 &&
          midi_duo_slot_note(&d, 1) == 48 &&
          d.key[1] == 48);
    check("Release II, then new note claims released slot during FADE",
          midi_duo_off(&d, 0, 48) == 1 &&
          midi_duo_on(&d, 0, 55, 36.0f, 48.0f) == 1);
    check("Releasing drone I leaves drone II's note intact",
          midi_duo_off(&d, 0, 36) == 0 &&
          midi_duo_slot_note(&d, 1) == 55);
    check("New note claims released drone I, not drone II",
          midi_duo_on(&d, 0, 43, 36.0f, 55.0f) == 0 &&
          d.key[1] == 55);
    check("Cross-channel release cannot stop assigned drone",
          midi_duo_off(&d, 1, 55) == -1 &&
          midi_duo_slot_note(&d, 1) == 55);

    midi_duo_reset(&d);
    midi_duo_on(&d, 0, 36, 36.0f, 48.0f);
    midi_duo_on(&d, 0, 60, 36.0f, 60.0f);
    check("Third note closer to drone I chooses drone I",
          midi_duo_on(&d, 0, 40, 36.0f, 60.0f) == 2 &&
          d.extra_slot == 0);
    check("Pitch-distance tie chooses drone I deterministically",
          (midi_duo_reset(&d),
           midi_duo_on(&d, 0, 36, 36.0f, 60.0f),
           midi_duo_on(&d, 0, 60, 36.0f, 60.0f),
           midi_duo_on(&d, 0, 48, 36.0f, 60.0f)) == 2);
    check("The nearest note can be chosen from live gliding pitches",
          (midi_duo_reset(&d),
           midi_duo_on(&d, 0, 36, 36.0f, 60.0f),
           midi_duo_on(&d, 0, 60, 36.0f, 60.0f),
           midi_duo_on(&d, 0, 54, 55.0f, 60.0f)) == 2);

    midi_duo_reset(&d);
    check("CC64 pedal-down engages sustain on its channel",
          midi_duo_pedal(&d, 0, 127) == 0 && (d.pedal & 1u));
    midi_duo_on(&d, 0, 36, 36.0f, 48.0f);
    midi_duo_on(&d, 0, 48, 36.0f, 48.0f);
    check("Note Off with pedal down retains both drone owners",
          midi_duo_off(&d, 0, 36) == -1 &&
          midi_duo_off(&d, 0, 48) == -1 &&
          d.key[0] == 36 && d.key[1] == 48 &&
          !d.down[0] && !d.down[1]);
    check("Pedal-up releases physically lifted notes together",
          midi_duo_pedal(&d, 0, 0) == 3u &&
          midi_duo_slot_note(&d, 0) == -1 &&
          midi_duo_slot_note(&d, 1) == -1);
    check("A fresh key after pedal release starts again on Drone I",
          midi_duo_on(&d, 0, 47, 36.0f, 48.0f) == 0);

    midi_duo_reset(&d);
    midi_duo_on(&d, 0, 36, 36.0f, 48.0f);
    midi_duo_on(&d, 0, 48, 36.0f, 48.0f);
    midi_duo_pedal(&d, 0, 90);
    midi_duo_on(&d, 0, 54, 36.0f, 48.0f);
    check("Sustain also latches third-note glide target",
          midi_duo_off(&d, 0, 54) == -1 &&
          midi_duo_slot_note(&d, 1) == 54 && !d.extra_down);
    check("Pedal-up removes released extra but keeps held originals",
          midi_duo_pedal(&d, 0, 0) == 2u &&
          midi_duo_slot_note(&d, 1) == 48 &&
          d.down[0] && d.down[1]);
    midi_duo_pedal(&d, 0, 127);
    midi_duo_off(&d, 0, 48);
    midi_duo_off(&d, 0, 36);
    check("Re-press while sustained counts as a physically held note",
          midi_duo_on(&d, 0, 36, 36.0f, 48.0f) == -1 &&
          d.down[0] && !d.down[1]);
    check("Pedal-up retains physically held drone I, releases II",
          midi_duo_pedal(&d, 0, 0) == 2u &&
          d.key[0] == 36 && d.key[1] == -1);
    check("Pedal channel isolation: CC64 on another channel irrelevant",
          midi_duo_pedal(&d, 2, 127) == 0u &&
          midi_duo_off(&d, 0, 36) == 0 && d.key[0] == -1);

    midi_duo_reset(&d);
    midi_duo_on(&d, 1, 36, 36.0f, 60.0f);
    midi_duo_on(&d, 1, 60, 36.0f, 60.0f);
    midi_duo_pedal(&d, 1, 127);
    midi_duo_off(&d, 1, 36);
    midi_duo_off(&d, 1, 60);
    check("All Notes Off cancels sustained keys on matching channel",
          midi_duo_channel_off(&d, 1) == 3u &&
          d.key[0] == -1 && d.key[1] == -1 && !d.pedal);
    check("Other-channel Note Off cannot alter empty slot",
          midi_duo_off(&d, 0, 36) == -1);
    printf(fail ? "midi-duo: %d FAILED\n" : "midi-duo: all ok\n", fail);
    return fail ? 1 : 0;
}
