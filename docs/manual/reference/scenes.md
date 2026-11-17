---
title: Scenes and the SAVE browser
intro: Store and recall sounds together with their programmed pitch sequences
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Scenes and the SAVE browser

A **scene** is a saved sound together with its authored pitch progression. Volcano has **48 writable scenes** (banks A–F, eight slots each) and **16 immutable factory presets** (two banks of eight). Press **[[SAVE]]** to manage them; the white keys are performance gestures, not scene pads.

> **Screenshot needed — SAVE Orbit browser:** Capture the current feature branch with the active bank, slot, action, and orbit nodes.

### Browser controls

| Control | Assignment |
|---|---|
| **KNOB 1** | Select bank: Presets 1–8, Presets 9–16, A, B, C, D, E, F |
| **KNOB 2** | Select slot 1–8 |
| **KNOB 3** | Select SAVE, LOAD or TRANSITION |
| **KNOB 4** | Set the target MORPH percentage for TRANSITION |
| **SAVE** | Confirm the selected action |
| **HOME** | Cancel or leave the browser |
| **REC** | Rename a user-scene slot |

The orbital browser moves from the two preset banks through A–F, with gradually brighter blue bank accents. This indicates the selected bank; it does not reflect the sound's modulation.

### SAVE: store your sound and sequence

1. Select a writable bank (A–F) and a slot.
2. Set KNOB 3 to **SAVE** and press SAVE.
3. If the slot already contains a scene, confirm **OVERWRITE?** with a second SAVE or cancel with HOME.

**Both the sound and its current sequencer pattern are stored**, including one to four event pitches, event spacing, transition shapes, selected drone, and drone II's phase offset. Factory presets cannot be overwritten.

> **Screenshot needed — Save and overwrite:** Show a selected user slot and the occupied-slot confirmation.

### LOAD: recall immediately

Select an occupied slot, choose **LOAD**, then press SAVE. The sound and its authored sequence are applied **immediately**, without using Scene Time. The progression is **stopped and reset to step 1**, ready to be restarted with **SEL + PLAY**. The saved pitches apply even if Pitch Lock is enabled. The browser identifies the action with **TIME: NOW**.

Loading a factory preset restores its sound with a **one-step** sequence. Loading an older user scene that has no stored sequence data likewise starts with one step. A user scene created on this feature branch can recall the multistep sequence you saved.

### TRANSITION: move toward another sound over time

Select a target, choose **TRANSITION**, set a percentage with KNOB 4, then press SAVE. This uses the configured **Scene Time** (or clock-synchronized bars), unlike LOAD.

- **0%** leaves the sound unchanged.
- **40%** reaches a sound 40% of the way toward the target.
- **100%** reaches the target sound.

**Partial transitions keep the current pitch sequence.** At **100%**, the target sequence is restored **only after the timed sound transition completes**, avoiding a pitch-sequence jump at the beginning. The destination sequence then starts stopped at step 1. See [Setup](setup.md) for Scene Time.

> **Screenshot needed — Partial transition:** Show TRANSITION at 40% and the Scene Time setting.

### Rename a user slot

Select the slot in the SAVE browser and press **REC**. KNOB 1 selects a character position; KNOB 2 changes the character. Press SAVE to confirm or HOME to cancel. Names are up to 11 characters; factory presets cannot be renamed.

## What a scene stores

| State | Stored in a user scene? |
|---|---|
| Oscillators, filters, routing, effects, sound parameters | **Yes** |
| Musical Tuning | **Yes** |
| One to four programmed pitches per drone, event intervals, transition shapes, selected drone, phase offset | **Yes** |
| Sequencer transport (running/paused), current playhead, editor cursor | **No** — recall starts stopped at step 1 |
| STRIP, DARK, STILL and TAIL amounts | **No** — performance/session state |
| Original eight gesture latch values | **Yes** |
| Recorded Motion Loop | **No** — performance state |
| Fade, Morph Time, Scene Time, MIDI Sync, stereo, Speaker, reference Tune and Pitch Lock | **No** — global instrument settings |

The scene format uses compatible sequence trailers for the six user banks; it does not require changing the original sound/project binary layout. **Still offsets remain active while sounds are loaded** but are not included in a saved scene.

**SEL + SAVE** offers undo/redo of the supported sound change; it should not be treated as a backup of your performed motion loop or sequence transport.

See [Scenes and saving](../recipes/scenes.md) for a complete exercise, [First pitch sequence](../recipes/sequencing.md) to program a progression, and [Presets](presets.md) for the factory sounds.
