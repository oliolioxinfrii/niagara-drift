---
title: Scenes and the SAVE browser
intro: Store, load, rename and transition between presets and saved sounds
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Scenes and the SAVE browser

A *scene* is a stored sound. VOLCANO includes **48 writable scenes** in six user banks (A–F), with eight slots per bank, and **16 fixed factory presets** in two further banks (P1–P8 and P9–P16).

**SAVE** is the scene browser. Choose a bank, a slot and an action with its encoders. On ordinary pages, white keys 1–8 are reserved; on SEQ → Sequence they enter notes.

> **Screenshot needed — Orbit scene browser:** `build/scenarios/volcano-ui/orbit-memory.png`.

## Open the scene browser

Tap **[[SAVE]]**. Its four encoders have fixed jobs:

| Knob | Control |
|---|---|
| **KNOB 1** | Bank: A, B, C, D, E, F, P1–P8, P9–P16 |
| **KNOB 2** | Slot: 1–8 |
| **KNOB 3** | Action: SAVE, LOAD or TRANSITION |
| **KNOB 4** | Transition amount: 0–100% (when using TRANSITION) |

Press **SAVE** to execute the selected action, or **HOME** to leave without executing it. **SEL** is not needed to navigate this browser.

## Save a sound

1. Set KNOB 1 to a user bank (A–F) and KNOB 2 to the desired slot.
2. Set KNOB 3 to **SAVE**, then press the SAVE button.
3. An empty slot is written immediately. If a slot already contains a sound, the instrument displays **OVERWRITE?**. Press SAVE again to replace it, or HOME to cancel.

Factory presets cannot be overwritten.

> **Screenshot needed — Overwrite confirmation:** Capture an occupied user-scene slot in the v1.5 SAVE browser.

## Load a sound

Choose a bank and an occupied slot, set the action to **LOAD**, then press SAVE. The sound changes over the configured **Scene time** ([Setup](setup.md)). You can also load factory presets from either Presets bank.

## Transition partway toward a sound

Choose **TRANSITION** and a target scene. Set KNOB 4 to the desired percentage and press SAVE. At **0%**, the current sound remains unchanged; at **100%**, it reaches the target sound. Values in between move the sound partway toward the target over Scene time. Adjusting the browser's encoders does not change the sound until SAVE is pressed.

A new transition starts from the sound currently being heard. Loading or transitioning to a factory preset resets the RAM-only pitch sequencer to one step.

> **Screenshot needed — Orbital scene transition:** Capture a SAVE browser target and transition value from the v1.5 build.

## Rename a user scene

Select a user bank and slot in the SAVE browser, then press **REC**. KNOB 1 chooses the character position and KNOB 2 changes the character (letters, numbers, space and hyphen). **SAVE** confirms; **HOME** cancels. Each user slot has its own name, up to 11 characters. Factory preset names cannot be changed.

## What a scene stores

The scene browser stores *a sound*, not an entire performance. This distinction is especially important for the pitch sequencer.

| Part of Volcano | Saved in a user scene? | Where to find it |
|---|---|---|
| Drone oscillators, filters, modulation routing, effects and other sound parameters | **Yes** | HOME, EDIT, ENV, LFO, FX and related controls |
| The selected musical **Tuning** | **Yes** | GLO → Setup 2 (Tuning follows scene recall) |
| Pitch-sequencer notes, event count, event spacing, phase offset and transition shapes | **No — RAM only** | SEQ → Sequence, Intervals and Transitions |
| A recorded performance **Motion Loop** | **No — performance memory** | REC |
| **Fade**, **Morph**, **Scene time**, MIDI Sync, Stereo, Speaker, reference **Tune** and **Pitch Lock** | **No — instrument settings** | GLO and the Pitch Lock key |

The instrument retains its system settings separately from scene slots. A factory-preset recall resets a multistep pitch sequence to one step, so it should not be used as a way to retrieve a composed sequence.

If you are constructing a performance, store the *sound* in a user scene, then program the pitch progression separately. See [First pitch sequence](../recipes/sequencing.md) for a worked example.

**SEL + SAVE** can restore the sound before the last supported scene change, then return to the later sound when used again.

See [Presets](presets.md) for the factory sounds, [Setup](setup.md) for Scene time and [Scenes recipes](../recipes/scenes.md) for examples.
