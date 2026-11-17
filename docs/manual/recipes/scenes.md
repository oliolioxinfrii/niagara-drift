---
title: Scenes and saving
intro: Use the SAVE browser to store, load, and blend sounds
description: Volcano v1.5: saving, loading, naming and partial transitions with the Orbit scene browser.
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

Volcano has six writable banks (A–F) of eight scenes each, plus 16 factory presets in two read-only banks. Everything is selected from the **SAVE browser**. User scenes preserve synthesis settings, Tuning **and their authored pitch sequence**: note pitches, event spacing, transition shapes, selected drone and phase offset. The running playhead and STRIP/DARK/STILL/TAIL amounts remain performance state.

### Make a preset your own {#save}

*Load a factory sound, shape it, then save it in your own bank.*

1. Press **[[SAVE]]** to open the browser. Set KNOB 1 to **Presets 1–8** or **Presets 9–16** and KNOB 2 to a preset slot.
2. Set KNOB 3 to **LOAD** and press SAVE to hear the preset **immediately**, without waiting for Scene Time.
3. Change the sound, and optionally [program a sequence](sequencing.md). Press SAVE again to reopen the browser.
4. Select user bank **A–F**, choose a slot, select **SAVE** as the action, and press SAVE to store **both sound and sequence**. An occupied slot requires a second SAVE to confirm overwriting; HOME cancels.

> **Screenshot needed — Orbit save browser:** `build/scenarios/volcano-ui/orbit-memory.png`, with a user bank and SAVE action selected.

### Transition between sounds {#blend}

*Move toward another sound by a chosen percentage.*

Volcano uses an explicit scene **TRANSITION** action for blending between saved sounds.

1. Start from any sounding scene or working sound.
2. Open SAVE and choose the target bank and slot.
3. Turn KNOB 3 to **TRANSITION**; set KNOB 4 to a percentage (for example, **40%**).
4. Press SAVE. Over the Scene Time, the sound approaches the chosen fraction of the target rather than loading it instantly.

At **0%**, the sound is unchanged. At a **partial** percentage, your current sequence remains active. At **100%**, the target's saved sequence is restored **after the sound transition has finished**; it starts stopped at step 1. You can transition again from the result.

> **Screenshot needed — Partial transition:** Capture the v1.5 scene browser showing a target and a non-100% transition setting.

### Rename a scene {#rename}

*Give an individual user slot a name.*

1. In the SAVE browser, select an existing user-bank slot.
2. Press **REC** to open its name editor.
3. Turn KNOB 1 to select a character position and KNOB 2 to change that character.
4. Press SAVE to keep the name, or HOME to cancel.

Names are attached to individual slots, not whole banks. Preset names cannot be changed.

> **Screenshot needed — Scene rename:** Capture the slot-name editor on a user scene.

### Undo a scene change {#back}

*Return to the previous sound after a scene change.*

**SEL + SAVE** returns to the sound before the last supported scene operation; use it again to move forward. HOME in the browser cancels the current browser action rather than changing the sound.

For exact encoder assignments and a table of what scenes store versus performance state, see [Scenes and the SAVE browser](../reference/scenes.md#what-a-scene-stores). To create and **save** a pitch progression with a sound, see [First pitch sequence](sequencing.md).
