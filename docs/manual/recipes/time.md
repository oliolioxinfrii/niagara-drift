---
title: Time and transitions
intro: Set the duration and shape of fades, scene transitions and sequences
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

# Time and transitions

Three timing controls appear on **GLO → Setup 1**: Fade (KNOB 1), Morph (KNOB 2), and Scene time (KNOB 3). Sync (KNOB 4) selects external MIDI-clock following.

> **Screenshot needed — Timing curves:** `build/scenarios/volcano-ui/setup-time-overlay.png`.

## Cut or glide between scenes {#cuts}

1. Open **GLO → Setup 1** and turn **Scene time** toward the shortest duration for an immediate change.
2. Open **SAVE**, choose an occupied scene, select **LOAD** and press SAVE.
3. Increase Scene time and load another scene to hear a slower change.

The scene browser can also choose **TRANSITION** and a percentage, ending partway toward a selected target sound instead of loading it completely.

> **Screenshot needed — Scene time and partial transition:** Capture Setup 1 and a scene-browser transition value from the v1.5 firmware.

## Make a switch move slowly {#morph}

1. On Setup 1, increase **Morph** with KNOB 2.
2. Tap a black key or change an LFO waveform. The parameter changes over the selected time.

Morph applies to parameter changes such as oscillator/sub controls and LFO wave selection; **Scene time** controls scene loading and transitions.

## Fade in slowly, or fade out one drone {#fades}

1. Increase **Fade** using Setup 1, KNOB 1.
2. Tap **PLAY** to fade the entire instrument in or out.
3. To fade only drone II, use **SEL + OCT+**. Use it again to bring the drone back.

**SEL + PLAY does not mute drone II** in Volcano v1.5; it starts, pauses or resumes the separate pitch sequencer.

## Change the spacing of a pitch sequence {#intervals}

1. Open **SEQ → Sequence** and program two to four notes.
2. Open **SEQ → Intervals** and set each event's relative arrival using KNOBs 1–3.
3. Adjust KNOB 4 to change the duration of the whole progression without changing those relative positions.

## Shape the pitch movement {#shape}

1. Open **SEQ → Transitions**.
2. Turn each active segment's knob (1→2, 2→3, 3→4 or 4→1).
3. Compare smooth easing with early and late abrupt pitch changes.

> **Screenshot needed — SEQ Intervals and Transitions:** Capture both v1.5 pages with a multistep programmed sequence.

For the complete sequencer controls, see [SEQ reference](../reference/partner.md). For scenes, see [Scenes and saving](scenes.md).
