---
title: MIDI
intro: Two external note-triggered drones, MIDI clock and Sync
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## MIDI

An external MIDI keyboard triggers the two drones, and MIDI clock synchronizes their movement, over USB or the TRS jack. The FM-1's own keys keep their performance-gesture and sequencer functions.

## MIDI in

### Notes

Incoming MIDI notes use **two independent drone slots**, regardless of MIDI channel. The first held key activates drone I; the second held key activates drone II. MIDI does not make VOLCANO polyphonic or change either drone's stored pitch.

**Each key release closes its drone using global FADE** (Setup 1). The next MIDI note reuses **whichever drone was released**: it waits for that drone to finish fading out, then begins the new pitch's FADE-in. The other held drone continues sounding.

For example, hold C2 to start drone I, then C3 to start drone II. Release C3 and press B3: drone II fades from C3 to silence, then fades in B3 while C2 continues. Release C2 and press B2 while holding B3: only drone I changes.

If a third key is pressed while both drones remain held, it **waits silently**. Whichever existing note is released first determines the slot the waiting key receives, after that drone's FADE-out. It never steals either held drone, and only two drones can sound at once. Releasing the waiting key before either slot becomes free cancels it.

- **Note On** starts a drone; **Note Off** releases it. A Note On with velocity zero is also Note Off.
- **Physical PLAY** exits external MIDI control and returns to VOLCANO's normal continuous drone transport.
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

- **Notes trigger**, rather than permanently tune, the two drones. PITCH LINK is temporarily bypassed and MIDI channels do not select the drone. FADE controls their attacks and releases.
- **A wobbly clock** is smoothed, and nothing drifts from it however long it runs.
- **Start** brings the Swell and the LFO back to their tops.
- **While synced,** scenes leave the Swell's on / off and division alone.
- **When the clock stops,** everything carries on at the last tempo ("KEEPS 120").
- **A delay note value** longer than the delay holds is halved.
- **In Logic:** File → Project Settings → Synchronization → MIDI, tick Clock for the FM-1.
- Recipe: [Play in time with a MIDI clock](../recipes/performing.md#clock)
