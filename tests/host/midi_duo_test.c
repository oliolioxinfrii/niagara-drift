/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include "midi_duo.h"
static int fail;
static void check(const char *s, int ok)
{
    printf("    %-57s %s\n", s, ok ? "ok" : "FAIL");
    fail += !ok;
}
int main(void)
{
    midi_duo_t d;
    midi_duo_reset(&d);
    check("First key C2 claims drone I", midi_duo_on(&d, 0, 36) == 0);
    check("Holding C2, second key C3 claims drone II",
          midi_duo_on(&d, 0, 48) == 1);
    check("Duplicate held Note On does not duplicate a voice",
          midi_duo_on(&d, 0, 48) == -1 && d.waiting == -1);
    check("Releasing C3 frees only drone II", midi_duo_off(&d, 0, 48) == 1 &&
          d.key[0] == 36 && d.key[1] == -1);
    check("B3 takes drone II while C2 remains held",
          midi_duo_on(&d, 0, 59) == 1);
    check("Releasing C2 leaves B3 untouched",
          midi_duo_off(&d, 0, 36) == 0 && d.key[1] == 59);
    check("B2 takes drone I while B3 remains held",
          midi_duo_on(&d, 0, 47) == 0);
    check("Third key C4 waits silently; neither note is stolen",
          midi_duo_on(&d, 0, 60) == -1 && d.waiting == 60 &&
          d.key[0] == 47 && d.key[1] == 59);
    check("Third key has no voice until an assigned key releases",
          midi_duo_promote(&d, 0) == -1 && d.waiting == 60);
    check("Releasing drone I assigns waiting C4 to drone I",
          midi_duo_off(&d, 0, 47) == 0 &&
          midi_duo_promote(&d, 0) == 60 &&
          d.key[0] == 60 && d.key[1] == 59 && d.waiting == -1);
    check("Releasing drone II gives next key that slot",
          midi_duo_off(&d, 0, 59) == 1 &&
          midi_duo_on(&d, 0, 61) == 1 && d.key[0] == 60);
    check("Wrong-channel note-off cannot release an occupied slot",
          midi_duo_off(&d, 1, 61) == -1 && d.key[1] == 61);
    check("All-notes-off releases the correct channel",
          midi_duo_channel_off(&d, 0) == 3u &&
          d.key[0] == -1 && d.key[1] == -1);

    midi_duo_reset(&d);
    midi_duo_on(&d, 0, 36);
    midi_duo_on(&d, 0, 48);
    midi_duo_on(&d, 0, 59);
    check("Reverse release order: C3 released -> waiting B3 takes II",
          midi_duo_off(&d, 0, 48) == 1 &&
          midi_duo_promote(&d, 1) == 59 && d.key[0] == 36);
    check("Released original C3 cannot stop new B3",
          midi_duo_off(&d, 0, 48) == -1 && d.key[1] == 59);

    midi_duo_reset(&d);
    midi_duo_on(&d, 0, 36);
    midi_duo_on(&d, 0, 48);
    midi_duo_on(&d, 0, 59);
    midi_duo_on(&d, 0, 47);
    check("Most recent waiting key supersedes the earlier third key",
          d.waiting == 47 && midi_duo_off(&d, 0, 59) == -1 &&
          d.waiting == 47);
    check("Newest waiting B2 enters whichever slot releases first",
          midi_duo_off(&d, 0, 36) == 0 &&
          midi_duo_promote(&d, 0) == 47);
    midi_duo_reset(&d);
    midi_duo_on(&d, 0, 36);
    midi_duo_on(&d, 0, 48);
    midi_duo_on(&d, 0, 59);
    check("Released waiting note cancels before it can sound",
          midi_duo_off(&d, 0, 59) == -1 && d.waiting == -1 &&
          midi_duo_off(&d, 0, 36) == 0 &&
          midi_duo_promote(&d, 0) == -1);

    midi_duo_reset(&d);
    check("First note on channel 10 still takes drone I",
          midi_duo_on(&d, 9, 47) == 0);
    check("Same pitch on another MIDI channel is a different note",
          midi_duo_on(&d, 10, 47) == 1);
    check("All-notes-off on channel 10 leaves channel 9 held",
          midi_duo_channel_off(&d, 10) == 2u && d.key[0] == 9*128+47);
    check("Wrong-channel note-off is ignored",
          midi_duo_off(&d, 10, 47) == -1 && d.key[0] == 9*128+47);
    check("Last note released leaves both slots available",
          midi_duo_off(&d, 9, 47) == 0 && d.key[0] == -1 && d.key[1] == -1);
    printf(fail ? "midi-duo: %d FAILED\n" : "midi-duo: all ok\n", fail);
    return fail ? 1 : 0;
}
