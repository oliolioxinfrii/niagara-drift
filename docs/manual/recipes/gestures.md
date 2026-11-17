---
title: Gestures
intro: Build and release sound changes without pressure or velocity
description: Hold-and-tap exercises for Volcano's sixteen performance gestures, including the per-drone subtractive controls.
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

All 16 white-key gestures share one interaction: **hold** to increase the amount, **release** to keep that amount, and **tap an active key** to begin returning toward zero. Tap again during the return to pause; another tap resumes. **PRESETS** changes **Morph Time**, which controls how long a 0–100% rise or return takes. On the Sequence page, the keyboard enters notes instead of gestures.

### Build a reverb wash, then release it {#hold}

*Wash can be held at any depth rather than automatically fading when you release the key.*

1. Load a sustained preset, such as **P1 Niagara**, from the SAVE browser.
2. Hold **Wash** ([[G5]], key 16) until the reverb has developed to the amount you want.
3. Release G5: Wash **stays at that amount**.
4. Tap G5 to start returning toward the original sound. The tail continues to decay as the gesture retreats.

> **Screenshot needed — Wash held at 60%:** Capture the gesture-percentage header from the current subtractive/STILL branch.

### Move into a chord gradually {#press}

*Chord depends on hold duration, not how hard the key is pressed.*

1. Load **P1 Niagara**. Open **SEQ → Pitch moves** and choose a Scale with KNOB 4.
2. Hold **Chord** ([[A4]], key 10) for part of the Morph Time; the partners move toward the chord.
3. Release at the amount you prefer, then tap once to return toward the starting harmony.

> **Screenshot needed — Chord partly applied:** Capture a retained percentage, not a pressure reading.

### Hold a particular amount, then pause its return {#keep}

*Every gesture retains the reached amount automatically.*

1. Hold **Weight** ([[D5]], key 13) until its amount reaches roughly 40%.
2. Release: it stays at 40%, without SEL or an extra latching action.
3. Tap Weight to begin returning toward zero. Tap during that return to pause the retreat; tap once more to resume it.
4. Alternatively, hold Weight for longer than about 220 ms to begin increasing from the retained amount again.

> **Screenshot needed — Retained and returning gestures:** Capture the header at a stationary partial amount and while returning.

### Remove harmonic content from one drone {#strip-dark}

*STRIP and DARK are subtractive gestures, not oscillator or filter switches.*

1. Start from a patch with both drones audible and some partner, sub or noise content.
2. Hold **STRIP I** (white key **1**) to reduce partner, sub and noise in drone I while leaving its main oscillator.
3. Release at a partial depth. Hold **DARK I** (white key **2**) to move drone I's tone toward sine and reduce drive/Character harmonics.
4. Use STRIP II (key **5**) or DARK II (key **6**) to compare the same effects on the other drone. Tap an active key to return toward the sound beneath it.

> **Screenshot needed — Drone I STRIP/DARK:** Capture both gesture percentages and the resulting waveform view.

### Stop one drone's movement, not its sound {#still}

*STILL is a local time control, not an audio mute.*

1. Program a two- or three-step pitch sequence using the [sequencer tutorial](sequencing.md), with both drones audible.
2. Start it with **SEL + PLAY**. Let the two drones progress together.
3. Hold **STILL II** (white key **7**) toward 100%. Drone II's **LFO, Swell and sequencer progression** slow independently until they stop at full depth. Its oscillators and existing effects remain audible; drone I continues progressing.
4. Release: the selected slowdown remains in effect. Tap key 7 to begin returning toward zero. As it returns, drone II's local phases smoothly catch up with the master clocks; they may briefly move faster than their normal rate.
5. Try **STILL I** (key **3**) as well, and listen to the two drones rejoin at different times.

> **Screenshot needed — Independent STILL clocks:** Capture SEQ Intervals and LFO progress markers with one drone stopped and the other running.

### Let a drone end while its effects continue {#tail}

*The delay and reverb buses are shared between the two drones.*

1. Load a patch with audible delay or reverb.
2. Hold **TAIL I** (white key **4**) until drone I contributes little new sound, then release to retain that amount.
3. Listen for the previous echoes and reverb to continue ringing. TAIL does **not** selectively delete them from the shared effects buffers.
4. Tap key 4 to restore the contribution over Morph Time. **TAIL II** (key **8**) works on drone II.

> **Screenshot needed — TAIL with effect decay:** Capture TAIL I percentage while the delay/reverb readouts remain visible.

### Stack two gestures {#stack}

*Independent amounts can create a changing composite effect.*

1. Hold **Snarl** ([[B4]], key 11) partway in and release to retain it.
2. Hold **Wash** ([[G5]], key 16) to add delay and reverb, and release.
3. Tap Snarl to withdraw its distortion while Wash remains; tap Wash when ready to withdraw the effects.

> **Screenshot needed — Snarl and Wash:** Capture one gesture's percentage, with the combined sound rendered separately when authentic scenario audio is available.

### Change how fast gestures move {#minute}

*Morph Time determines how long a full gesture movement takes.*

1. Set **GLO → Setup 1 → Morph** to a long duration, or turn **PRESETS** to the same parameter.
2. Hold a gesture such as Weight until it reaches the amount you want. Release to retain it.
3. Tap Weight to start a correspondingly slow return.

> **Screenshot needed — Morph Time:** Capture Setup 1 with a long Morph setting and the gesture percentage.

### Speed a gesture up, or pause a return {#speed}

*Changing Morph Time affects gesture movement without changing its saved sound settings.*

1. Use a shorter Morph Time and hold **Wash** to raise it quickly.
2. Release to retain the amount. Increase Morph Time, then tap Wash: its return now takes longer.
3. Tap again mid-return to hold that intermediate amount. Tap once more to continue back toward zero.

> **Screenshot needed — Wash returning slowly:** Capture a gesture percentage during a paused return.

> **Audio previews pending:** These examples need to be recorded from the subtractive/STILL firmware; previous pressure-gesture recordings no longer demonstrate the current interaction.

See [Gestures reference](../reference/gestures.md) for the full key map and [Scenes](../reference/scenes.md#what-a-scene-stores) for which gesture amounts are saved.
