---
title: Scenes and banks
intro: The white keys: your sounds, gliding or crossfaded in by pressure
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Scenes and banks — Volcano development branch

On the Volcano branch, **white keys 1–16 are gestures** except during SEQ
keyboard note entry. Scenes are stored and recalled in the **SAVE browser**,
not by pressing a scene key. This replaces the older white-key scene controls
documented further down this page.

Tap **SAVE** to open the browser. K1 selects the bank (A–F or the two factory
Preset banks), K2 selects the slot, and K3 chooses **Save**, **Load**, or
**Transition**. K4 chooses the final morph percentage only for Transition.
Press SAVE to confirm, or HOME to exit; overwriting an occupied user slot
requires a second SAVE confirmation.

- **Save** stores the current sound **and its authored 1–4-step sequence**
  in a user bank A–F. Factory Presets cannot be overwritten.
- **Load** immediately applies the selected sound and sequence without
  waiting for Scene Time. The sequence returns to step 1, stopped; room
  settings remain global. This is intended for starting or returning to an
  idea before performance.
- **Transition** moves toward the target sound over the configured Scene
  Time, ending at the K4 percentage. Partial transitions retain the current
  sequence; a 100% transition switches to the target sequence at the end of
  the timed sound change.

## Earlier scene-key controls (historical)

The following shortcuts apply to earlier Niagara Drift versions, **not**
to the Volcano development branch.

The white keys play your saved sounds, gliding in, or crossfading as you press.

## Scenes

<img src="../../img/panel-keys.png" width="720" alt="The FM-1's keys: white keys 1 to 8 are scenes; the eight gesture keys above them each pick a bank with SEL (A to F, P1–8, P9–16); the black keys are switches">

White keys 1–8 hold scenes, in 8 banks of 8: A to F are yours, and two hold the Presets.

### Tap

Glides to the scene, and stays.

### Press

Crossfades toward the scene, as far as you press. Let go and it glides back.

### Keep a blend

Tap [[SEL]] while pressing: the blend stays.

### Take it

Press all the way and hold a moment: the scene stays when you let go.

### Undo

[[SEL]] + [[SAVE]]: back to the sound before. Again: forward.

## Banks

### Save

Hold [[SAVE]], tap a key.

### Pick a bank

[[SEL]] + a gesture key: [[G4]] A, [[A4]] B, [[B4]] C, [[C5]] D, [[D5]] E, [[E5]] F; [[F5]] P1–P8, [[G5]] P9–P16.

## Good to know

- **The glide** takes the Scene time (Setup 1). Turn a knob to stop it part way.
- **A tap** is anything let go within half a second, however hard.
- **The screen** says how far a press has gone ("A5 40%"), "KEPT" for a kept blend, and "→ A5" as it glides back.
- **SAVE first:** a key pressed before SAVE plays its scene. The Presets can't be saved over.
- **A scene keeps the sound and its Tuning,** not the room: Pressure, Stereo, Tune, Sync, Pitch lock and the Setup
  times stay.
- **The keys' lights:** lit keys hold a scene; the one you're on blinks slowly until you change something.
- **Keys 9–16** are the [gestures](gestures.md).
- **From 0.2:** scenes on keys 9–12 moved to empty keys 1–8 (the screen says how many). 0.2's Presets that 0.3
  doesn't keep (Eerie, Glass, Tanpura, Whistle, Cathedral, Metal) are in bank F, keys 1–6; Heaven is now Sibyl, Clean
  is Bare. Feedback moved to KNOB 1 on Life, Cross-mod to KNOB 4 on Mods, Tune to KNOB 2 on Setup 2; HOME opens
  Drones, FX opens Space.
