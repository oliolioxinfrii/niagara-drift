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
- **MIDI mode:** Selecting it starts the two LFO transport clocks and the Swell immediately, without starting either drone or the pitch sequence. Both drone voices stay silent until an external MIDI key is pressed. The Swell runs even when the saved patch has Swell OFF; that saved switch is not changed. SEQ's **Sequence, Intervals and Transitions** pages are dimmed and read-only; turning their knobs displays **SEQUENCER DISABLED**. SEL+PLAY cannot start the sequence. Partner and Pitch moves still work, and SEL+OCT does not override MIDI drone gates.
- **PLAY while in MIDI mode:** stops or restarts acceptance of triggered notes but does **not** change modes. Stopping releases held MIDI notes. A restarted MIDI transport has no sounding voices until another Note On.
- **SEL + GLO again:** leaves MIDI mode, fades to silence and returns to normal PLAY mode, initially stopped. Use PLAY to start the normal continuous drones.

The FM-1's physical white and black keys keep their original gestures, switches, and GHOST excitation actions in both modes. The GLO button's ordinary setup-page function is unchanged unless SEL is held.

### Notes

**Only in MIDI mode**, incoming MIDI notes use **two independent drone slots**, regardless of MIDI channel. The first held key activates drone I; the second held key activates drone II. MIDI does not make VOLCANO polyphonic or change either drone's stored pitch.

**Each key release closes its drone using global FADE** (Setup 1). The next MIDI note reuses **whichever drone was released**: it waits for that drone to finish fading out, then begins the new pitch's FADE-in. The other held drone continues sounding.

For example, hold C2 to start drone I, then C3 to start drone II. Release C3 and press B3: drone II fades from C3 to silence, then fades in B3 while C2 continues. Release C2 and press B2 while holding B3: only drone I changes.

If a **third MIDI key is held while both drones are already sounding**, VOLCANO finds the drone **closest in its current sounding pitch** and glides that existing drone toward the third key's pitch over the global **Scene Time** (Setup 1). This produces **no third voice**. If the two drones are equidistant, **the drone whose note was played most recently** is selected.

Each drone has its own glide state. While Drone II is gliding, a fourth note that is closest to Drone II is **ignored**, rather than interrupting its glide or being redirected to Drone I. However, a fourth note closest to Drone I can start a **separate, simultaneous glide** of Drone I. Ignored notes must be released and pressed again to be considered after a glide ends.

Without the sustain pedal, the originally assigned key remains an owner during a glide. Releasing that original key does **not** interrupt a transition toward a held extra key. When the transition finishes and the original has been released, the held destination becomes the new primary. If an extra key is released first, the drone glides back to the still-held original. If neither is held, the drone fades out using FADE.

**With the sustain pedal (CC64) held**, a completed glide destination becomes the new primary **even if the older note is still physically held or pedal-latched**. The next MIDI Note On after completion can therefore begin another glide indefinitely. A note directed at a drone whose glide has not finished is still ignored. **On pedal release**, each affected drone retains a note only when the **exact current pitch or active glide destination** is still physically held. If that key is no longer down, the glide stops and its drone fades out, even if an older, superseded note is still physically held. Subsequent note presses again follow normal released-slot FADE and held-note glide rules.

This differs from a new note played **after a drone key is released**: that ordinary replacement uses the released slot, waits until the previous note has completed its FADE-out, then retunes silently and fades in. This holds even when the new Note On arrives during the preceding fade.

- **Note On** starts a drone; **Note Off** releases it. A Note On with velocity zero is also Note Off.
- **Sustain pedal (CC64)** latches released notes and enables chained glides after each Scene-Time transition completes. On CC64 falling below 64, only the **exact currently sounding or destination note** can survive if its key is physically held. Old held notes cannot pull the drone back. Sustain is interpreted independently per incoming MIDI channel.
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
