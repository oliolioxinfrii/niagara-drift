---
title: Time and transitions
intro: Set the duration and shape of fades, scene transitions and sequences
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

Three timing controls appear on **GLO → Setup 1**: Fade (KNOB 1), Morph (KNOB 2), and Scene time (KNOB 3). Sync (KNOB 4) selects external MIDI-clock following.

> **Screenshot needed — Timing curves:** `build/scenarios/volcano-ui/setup-time-overlay.png`.

### Cut or glide between scenes {#cuts}

*Use Scene time to change the pace of recalling a sound.*

1. Open **GLO → Setup 1** and set **Scene Time** to a short duration to compare fast and slow performance transitions.
2. Open **SAVE**, choose an occupied scene, select **TRANSITION** at 100% and press SAVE.
3. Increase Scene Time and repeat a 100% TRANSITION to hear a slower movement. Choose **LOAD** instead for an immediate edit/startup recall.

**LOAD** applies a scene immediately, regardless of Scene Time. To hear Scene Time, select **TRANSITION** and a percentage. A partial transition retains the working sequence; a 100% transition loads the target sequence only after the sound change completes.

> **Screenshot needed — Scene time and partial transition:** Capture Setup 1 and a scene-browser transition value from the v1.5 firmware.

### Make a switch move slowly {#morph}

*Set how gradually switches and LFO waveform changes take effect.*

1. On Setup 1, increase **Morph** with KNOB 2.
2. Tap a black key or change an LFO waveform. The parameter changes over the selected time.

Morph applies to parameter changes such as oscillator/sub controls and LFO wave selection, **all 16 gesture rise/return movements**, and STILL rejoining. **SEL + PRESETS** controls the same Morph Time setting as GLO Setup 1. **Scene Time** controls TRANSITION, not LOAD.

### Fade in slowly, or fade out one drone {#fades}

*Change the overall volume or an individual drone without retuning.*

1. Increase **Fade** using Setup 1, KNOB 1.
2. Tap **PLAY** to fade the entire instrument in or out.
3. To fade only drone II, use **SEL + OCT+**. Use it again to bring the drone back.

**SEL + PLAY does not mute drone II** in Volcano v1.5; it starts, pauses or resumes the separate pitch sequencer.

### Change the spacing of a pitch sequence {#intervals}

*Place successive notes at unequal times in the loop.*

1. [Program two to four notes](sequencing.md#first-sequence) on **SEQ → Sequence**.
2. Open **SEQ → Intervals** and set each event's relative arrival using KNOBs 1–3.
3. Adjust KNOB 4 to change the duration of the whole progression without changing those relative positions.

### Shape the pitch movement {#shape}

*Make changes between notes smooth, early or late.*

1. Open **SEQ → Transitions** after programming at least two notes.
2. Turn the knobs for the active segments. With three notes, KNOBs 1–3 shape **1→2, 2→3 and 3→1**; with four notes, KNOB 4 also shapes **4→1**.
3. Compare smooth easing with early and late abrupt pitch changes.

> **Screenshot needed — SEQ Intervals and Transitions:** Capture both v1.5 pages with a multistep programmed sequence.

For the complete sequencer controls, see [SEQ reference](../reference/partner.md). For scenes, see [Scenes and saving](scenes.md).
