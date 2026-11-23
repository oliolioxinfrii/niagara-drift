/* SPDX-License-Identifier: GPL-3.0-only */
/* Two independently gliding MIDI drone slots, note ownership and CC64.
 * DSP completion is represented by explicit promote_completed() calls;
 * the end-to-end host scenarios verify the actual audio countdown. */
#include <stdio.h>
#include "midi_duo.h"
static int failures;
static void test(const char *label, int ok)
{
    printf("    %-67s %s\n", label, ok ? "ok" : "FAIL");
    failures += !ok;
}
int main(void)
{
    midi_duo_t d;
    midi_duo_reset(&d);
    test("First A2 claims Drone I", midi_duo_on(&d,0,45,45,79,0,0)==0);
    test("Second G5 claims Drone II",midi_duo_on(&d,0,79,45,79,0,0)==1);
    test("A5 as third goes to nearest G5 (Drone II)",
         midi_duo_on(&d,0,81,45,79,0,0)==3 && d.extra[1]==81 &&
         d.extra[0]<0 && d.key[1]==79);
    test("B5 during Drone II glide is ignored (no rerouting to I)",
         midi_duo_on(&d,0,83,45,80,0,1)==-1 &&
         d.extra[0]<0 && d.extra[1]==81 && d.last_slot==1);
    test("Ignored B5 Note Off does not change either drone",
         midi_duo_off(&d,0,83)==-1 && d.extra[1]==81);
    test("Releasing original G5 does not disturb A5's glide",
         midi_duo_off(&d,0,79)==1 && d.key[1]<0 &&
         midi_duo_slot_note(&d,1)==81);
    test("B5 still ignored until DSP says glide is complete",
         midi_duo_on(&d,0,83,45,81,0,1)==-1 && d.extra[1]==81);
    test("DSP completion promotes A5 to the new Drone II primary",
         midi_duo_promote_completed(&d,1)==1 && d.key[1]==81 &&
         d.down[1] && d.extra[1]<0);
    test("Re-pressed B5 now glides A5 -> B5 on Drone II",
         midi_duo_on(&d,0,83,45,81,0,0)==3 &&
         d.key[1]==81 && d.extra[1]==83);
    test("Release A5 while B5 glides: uninterrupted",
         midi_duo_off(&d,0,81)==1 && d.key[1]<0 &&
         midi_duo_slot_note(&d,1)==83);
    test("Second completed glide promotes B5 to primary",
         midi_duo_promote_completed(&d,1)==1 && d.key[1]==83 &&
         d.extra[1]<0);
    test("Further transitions remain possible indefinitely",
         midi_duo_on(&d,0,84,45,83,0,0)==3 && d.extra[1]==84);
    test("No additional oscillator or primary MIDI slot is created",
         d.key[0]==45 && d.key[1]==83);

    midi_duo_reset(&d);
    midi_duo_on(&d,0,45,45,79,0,0);
    midi_duo_on(&d,0,79,45,79,0,0);
    midi_duo_on(&d,0,81,45,79,0,0); /* II busy */
    test("B2 goes to the nearer free-to-glide Drone I",
         midi_duo_on(&d,0,47,45,80,0,1)==2 &&
         d.extra[0]==47 && d.extra[1]==81);
    test("Both drones carry independent pitch targets, still two voices",
         midi_duo_slot_note(&d,0)==47 && midi_duo_slot_note(&d,1)==81);
    test("Further key closest to busy II ignored, I's glide unchanged",
         midi_duo_on(&d,0,82,46,80,1,1)==-1 &&
         d.extra[0]==47 && d.extra[1]==81);
    test("When original I is released midglide, extra remains",
         midi_duo_off(&d,0,45)==0 && d.extra[0]==47 && d.key[0]<0);
    test("When original II is released midglide, extra remains",
         midi_duo_off(&d,0,79)==1 && d.extra[1]==81 && d.key[1]<0);
    test("Two completed glides independently promote their held targets",
         midi_duo_promote_completed(&d,0)==1 &&
         midi_duo_promote_completed(&d,1)==1 &&
         d.key[0]==47 && d.key[1]==81);
    test("New nearby notes can now start either drone's next glide",
         midi_duo_on(&d,0,48,47,81,0,0)==2 &&
         midi_duo_on(&d,0,82,47,81,1,0)==3);

    midi_duo_reset(&d);
    midi_duo_on(&d,0,45,45,79,0,0);
    midi_duo_on(&d,0,79,45,79,0,0);
    test("Equidistant third note selects last-pressed G5 / Drone II",
         midi_duo_on(&d,0,62,45,79,0,0)==3 &&
         d.last_slot==1 && d.extra[1]==62);
    midi_duo_reset(&d);
    midi_duo_on(&d,0,45,45,79,0,0);
    midi_duo_on(&d,0,79,45,79,0,0);
    midi_duo_off(&d,0,45);
    midi_duo_on(&d,0,45,45,79,0,0); /* newest Note On belongs to I */
    test("Equal distance now chooses the newly played Drone I",
         midi_duo_on(&d,0,62,45,79,0,0)==2 &&
         d.last_slot==0 && d.extra[0]==62);
    test("A pitch equally close to a BUSY last-played drone is ignored",
         midi_duo_on(&d,0,63,46,80,1,0)==-1 &&
         d.extra[1]<0 && d.extra[0]==62);

    midi_duo_reset(&d);
    midi_duo_on(&d,0,45,45,79,0,0);
    midi_duo_on(&d,0,79,45,79,0,0);
    midi_duo_on(&d,0,81,45,79,0,0);
    test("Releasing extra before original returns to G5",
         midi_duo_off(&d,0,81)==3 &&
         midi_duo_slot_note(&d,1)==79 && d.extra[1]<0);
    test("Return glide blocks another near-II key until it completes",
         midi_duo_on(&d,0,82,45,80,0,1)==-1 && d.extra[1]<0);
    test("After return completes, a new near-II glide is possible",
         midi_duo_on(&d,0,82,45,79,0,0)==3 && d.extra[1]==82);
    test("If both owner and extra release, drone II becomes free",
         midi_duo_off(&d,0,79)==1 &&
         midi_duo_off(&d,0,82)==3 &&
         midi_duo_slot_note(&d,1)==-1);
    test("Next key reuses released II regardless of its pitch",
         midi_duo_on(&d,0,30,45,79,0,0)==1 &&
         d.key[0]==45 && d.key[1]==30);

    midi_duo_reset(&d);
    midi_duo_on(&d,0,45,45,79,0,0);
    midi_duo_on(&d,0,79,45,79,0,0);
    midi_duo_on(&d,0,81,45,79,0,0);
    test("CC64 down latches physical note releases",
         midi_duo_pedal(&d,0,127)==0 &&
         midi_duo_off(&d,0,79)==-1 && d.key[1]==79);
    test("CC64 down also sustains a glide target",
         midi_duo_off(&d,0,81)==-1 &&
         d.extra[1]==81 && !d.extra_down[1]);
    test("Pedal-up releases both lifted keys, not the held A2",
         midi_duo_pedal(&d,0,0)==2 &&
         d.key[0]==45 && d.key[1]<0 && d.extra[1]<0);
    test("A released slot is available to normal FADE-separated retrigger",
         midi_duo_on(&d,0,50,45,81,0,0)==1 && d.key[1]==50);

    midi_duo_reset(&d);
    midi_duo_on(&d,0,45,45,79,0,0);
    midi_duo_on(&d,0,79,45,79,0,0);
    midi_duo_on(&d,0,81,45,79,0,0);
    midi_duo_pedal(&d,0,127);
    midi_duo_off(&d,0,79); /* sustained original */
    test("Completed glide cannot promote while original is pedal-held",
         midi_duo_promote_completed(&d,1)==0 &&
         d.key[1]==79 && d.extra[1]==81);
    test("Pedal-up releases old key but leaves physically held A5",
         midi_duo_pedal(&d,0,0)==2 &&
         midi_duo_promote_completed(&d,1)==1 &&
         d.key[1]==81 && d.extra[1]<0);
    test("New target may glide immediately after that promotion",
         midi_duo_on(&d,0,83,45,81,0,0)==3);

    midi_duo_reset(&d);
    midi_duo_on(&d,2,45,45,79,0,0);
    midi_duo_on(&d,2,79,45,79,0,0);
    midi_duo_on(&d,2,81,45,79,0,0);
    midi_duo_pedal(&d,2,127);
    midi_duo_off(&d,2,45);
    midi_duo_off(&d,2,79);
    test("CC123 releases both primaries and extra on that channel",
         midi_duo_channel_off(&d,2)==3 &&
         d.key[0]<0 && d.key[1]<0 && d.extra[1]<0 &&
         d.pedal==0);
    printf(failures ? "midi-duo: %d FAILED\n" : "midi-duo: all ok\n", failures);
    return failures ? 1 : 0;
}
