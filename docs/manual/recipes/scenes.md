---
title: Scenes and saving
intro: Use the SAVE browser to store, load, and blend sounds
description: Volcano v1.5: saving, loading, naming and partial transitions with the Orbit scene browser.
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

# Scenes and saving

Volcano has six writable banks (A–F) of eight scenes each, plus 16 factory presets in two read-only banks. Everything is selected from the **SAVE browser**; ordinary white keys 1–8 no longer load or store scenes.

### Make a preset your own {#save}

1. Press **[[SAVE]]** to open the browser. Set KNOB 1 to **Presets 1–8** or **Presets 9–16** and KNOB 2 to a preset slot.
2. Set KNOB 3 to **LOAD** and press SAVE to hear the preset.
3. Change the sound as you like, then press SAVE again to reopen the browser.
4. Select user bank **A–F**, choose a slot, select **SAVE** as the action, and press SAVE to store it. An occupied slot requires a second SAVE to confirm overwriting; HOME cancels.

> **Screenshot needed — Orbit save browser:** `build/scenarios/volcano-ui/orbit-memory.png`, with a user bank and SAVE action selected.

### Transition between sounds {#blend}

Instead of pressure-crossfading on a white key, Volcano uses an explicit scene **TRANSITION** action.

1. Start from any sounding scene or working sound.
2. Open SAVE and choose the target bank and slot.
3. Turn KNOB 3 to **TRANSITION**; set KNOB 4 to a percentage (for example, **40%**).
4. Press SAVE. Over the Scene time, the sound approaches the chosen fraction of the target rather than replacing the starting sound outright.

At 100% the transition reaches the complete target sound. At 0% the sound stays unchanged. You can transition again from the result.

> **Screenshot needed — Partial transition:** Capture the v1.5 scene browser showing a target and a non-100% transition setting.

### Rename a scene {#rename}

1. In the SAVE browser, select an existing user-bank slot.
2. Press **REC** to open its name editor.
3. Turn KNOB 1 to select a character position and KNOB 2 to change that character.
4. Press SAVE to keep the name, or HOME to cancel.

Names are attached to individual slots, not whole banks. Preset names cannot be changed.

> **Screenshot needed — Scene rename:** Capture the slot-name editor on a user scene.

### Undo a scene change {#back}

**SEL + SAVE** returns to the sound before the last supported scene operation; use it again to move forward. HOME in the browser cancels the current browser action rather than changing the sound.

For exact encoder assignments, see [Scenes and the SAVE browser](../reference/scenes.md).
