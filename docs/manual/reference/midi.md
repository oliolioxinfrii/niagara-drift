---
title: MIDI
intro: Two external note-triggered drones, MIDI clock and Sync
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## MIDI

An external MIDI keyboard triggers the two drones, and MIDI clock synchronizes their movement, over USB or the TRS jack. The FM-1's own keys keep their performance-gesture and sequencer functions.

## MIDI in

### Performance modes: SEL + GLO

**SEL + GLO** toggles between VOLCANO's two performance modes. This choice is session-only and is not saved into scenes or presets.

- **PLAY mode (normal):** PLAY fades the full instrument in or out; the sequencer remains available via SEL+PLAY. Each drone can be independently faded out or in with SEL+OCT− / SEL+OCT+. Incoming external MIDI notes do not retune or trigger the drones in this mode.
- **MIDI mode:** Selecting it starts the two LFO transport clocks and the Swell immediately, without starting either drone or the pitch sequence. Both drone voices stay silent until an external MIDI key is pressed. The Swell runs even when the saved patch has Swell OFF; that saved switch is not changed. SEL+PLAY cannot start the sequence in MIDI mode. SEL+OCT does not override the MIDI-controlled drone gates.
- **PLAY while in MIDI mode:** stops or restarts acceptance of triggered notes but does **not** change modes. Stopping releases held MIDI notes. A restarted MIDI transport has no sounding voices until another Note On.
- **SEL + GLO again:** leaves MIDI mode, fades to silence and returns to normal PLAY mode, initially stopped. Use PLAY to start the normal continuous drones.

The FM-1's physical white and black keys keep their original gestures, switches, and GHOST excitation actions in both modes. The GLO button's ordinary setup-page function is unchanged unless SEL is held.

### Notes

**Only in MIDI mode**, incoming MIDI notes use **two independent drone slots**, regardless of MIDI channel. The first held key activates drone I; the second held key activates drone II. MIDI does not make VOLCANO polyphonic or change either drone's stored pitch.

**Each key release closes its drone using global FADE** (Setup 1). The next MIDI note reuses **whichever drone was released**: it waits for that drone to finish fading out, then begins the new pitch's FADE-in. The other held drone continues sounding.

For example, hold C2 to start drone I, then C3 to start drone II. Release C3 and press B3: drone II fades from C3 to silence, then fades in B3 while C2 continues. Release C2 and press B2 while holding B3: only drone I changes.

If a **third MIDI key is held while both drones are already sounding**, VOLCANO finds the drone **closest in its current sounding pitch** and glides that existing drone toward the third key's pitch over the global **Scene Time** (Setup 1). This produces **no third voice** and does not interrupt the other drone. If both drones are equally close, Drone I is selected.

The originally assigned key remains an owner during the glide. Releasing that original key does **not** interrupt the transition while the third key is held. Releasing the third key while the original remains held starts a new Scene-Time glide back to the original pitch. When neither key remains held or sustained, that drone stops transitioning and fades out using global **FADE**. A fourth overlapping key cannot steal or add a voice.

This differs from a new note played **after a drone key is released**: that ordinary replacement uses the released slot, waits until the previous note has completed its FADE-out, then retunes silently and fades in. This holds even when the new Note On arrives during the preceding fade.

- **Note On** starts a drone; **Note Off** releases it. A Note On with velocity zero is also Note Off.
- **Sustain pedal (CC64)** acts as a temporary latch for MIDI notes. With CC64 at 64 or above, lifting a key does not release its drone or a third-note glide target. When CC64 falls below 64, notes whose keys are physically up are released; keys still held remain active. Sustain is interpreted independently for each MIDI channel.
- **SEL+GLO** exits external MIDI control. PLAY starts or stops the current performance mode without changing it.
- Each drone's patch, filters, GHOST interactions and effects remain in force. Saved project pitch parameters do not change.
- PITCH LINK is temporarily bypassed while MIDI controls both pitches independently; the saved setting is preserved.
- A sustained key continues the note, and subsequent notes are triggers rather than a continuous pitch-following keyboard. No additional oscillator voices are introduced.

### Clock

A MIDI clock from a DAW or a drum machine. After about two beats it locks ("120 BPM").

### Sync

Setup 1, KNOB 4: on follows a clock, off ignores it.

## On a clock

| What | On the clock |
|---|---|
| **The Swell** (KNOB 1 on its page) | a breath a **Division**: 32 bars to 1/4 |
| **The LFO** (SEL + SELECT) | a cycle a division: 32 bars to 1/16 ("LFO CYCLE") |
| **The delay** (Time, Details) | a **note value**: 1/16, 1/8T, 1/8, 1/4T, 1/8D, 1/4, 1/2T, 1/4D, 1/2, 1/2D, 1 bar |
| **Wander** | its notes land on the beat (on the bar, when slow), still at random |
| **Scene Time** (Setup 1) | in **bars**, 1 to 16, for TRANSITION; **LOAD** remains immediate |

**STILL** can slow the effective LFO, Swell and sequencer clocks of either drone even when master clocks remain synchronized. When STILL returns to zero, the local phases smoothly rejoin. The **SEQ pitch progression** uses its own loop duration in seconds. The Sync setting organizes Swell, LFO and delay timing around the external MIDI clock, but does not convert the sequencer's event positions into a clocked grid. For a free-time sequencing example, see [First pitch sequence](../recipes/sequencing.md).

## Good to know

- **In MIDI mode**, notes trigger rather than permanently tune the two drones. PITCH LINK is temporarily bypassed and MIDI channels do not select the drone. FADE controls their attacks and releases.
- **A wobbly clock** is smoothed, and nothing drifts from it however long it runs.
- **Start** brings the Swell and the LFO back to their tops.
- **While synced,** scenes leave the Swell's on / off and division alone.
- **When the clock stops,** everything carries on at the last tempo ("KEEPS 120").
- **A delay note value** longer than the delay holds is halved.
- **In Logic:** File → Project Settings → Synchronization → MIDI, tick Clock for the FM-1.
- Recipe: [Play in time with a MIDI clock](../recipes/performing.md#clock)
