---
title: First pitch sequence
intro: Build an uneven three-note progression, then add a second drone
description: A guided introduction to Volcano's pitch sequencer, event positions and transition shapes.
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

Volcano sequences **pitches, not note-on events**. The drones continue sounding while the sequencer changes their pitches. A loop may contain one to four events, and the time between them need not be equal.

### Make a three-note progression {#first-sequence}

*Program three notes, give them different lengths, and shape the movement between them.*

1. Start with a simple sound: open **SAVE**, choose **Presets 9–16**, slot **8 (Bare)**, and action **LOAD**, then press SAVE. Press **PLAY** if the instrument is silent.
2. Tap **SEQ** until **Sequence** appears (page 3 of 5). Turn **KNOB 3** until the display shows **3 steps**. **KNOB 1** selects the step being edited.
3. Select **step 1** with KNOB 1 and press **A3** on the physical keyboard. This also sets drone I's live pitch on **HOME → Pitch**; step 1 always follows that live pitch.
4. Select **step 2** and press **C4**, then select **step 3** and press **E4**. On the Sequence page, *all* black and white piano keys enter notes rather than triggering their usual switches or gestures.
5. Open **SEQ → Intervals** (page 4). Turn **KNOB 4** to shorten the complete loop to about **13 seconds**. The relative positions of the notes stay in place as the loop changes length.
6. Use **KNOB 1** to place step 2 near **3 seconds**, and **KNOB 2** to place step 3 near **9 seconds**. The resulting gaps are approximately **3, 6 and 4 seconds**, including the return to step 1. KNOB 3 has no effect on a three-step sequence.
7. Open **SEQ → Transitions** (page 5). With three steps, KNOB 1 shapes **1→2**, KNOB 2 shapes **2→3**, and KNOB 3 shapes **3→1**. Try a smooth midpoint for the first transition, an *early* change (positive shape) for the second, and a *late* change (negative shape) for the return.
8. Hold **SEL** and tap **PLAY** to start the pitch progression. Repeat this combination to pause and resume without muting the drones. Normal PLAY controls the instrument's overall fade and stops/resets the progression after fade-out.

The three pitches now repeat, but the time spent traveling between them is unequal. The transition shapes determine *when within each interval* the pitch moves, not just how long the interval lasts.

> **Screenshot needed — Sequence notes:** Capture the v1.5 SEQ Sequence page with three programmed notes and step 3 selected.

> **Screenshot needed — Unequal intervals:** Capture SEQ Intervals with a 13-second loop and events near 0, 3 and 9 seconds.

> **Screenshot needed — Three transition shapes:** Capture SEQ Transitions with three non-identical active segments.

### Give drone II its own progression {#second-drone}

*Add separate pitches for the second drone, then offset it in time.*

1. Return to **HOME → Pitch**. If **Pitch Lock** (C♯4) or **Pitch Link** (D♯4) is enabled, switch each off using its black key on this ordinary page. Either link mode makes drone II follow drone I's programmed notes.
2. If drone II is muted, hold **SEL** and tap **OCT+** to bring it back.
3. Return to **SEQ → Sequence**. Turn **KNOB 2** to select **drone II**, use KNOB 1 to select each event, and enter its pitch on the keyboard.
4. Turn **KNOB 4** to shift drone II's phase around the loop. For example, an offset of 50% places it half a cycle away from drone I's corresponding events. The drones share the same overall event-spacing pattern, but need not play the same pitches or start at the same point.

**Important:** The sequencer lives in RAM. **SAVE stores the sound, not these note and timing edits.** A scene cannot be used to recall a completed pitch progression; factory preset recall resets a multistep sequence to one step.

> **Screenshot needed — Two-drone sequence:** Capture the Sequence page with drone II selected and a nonzero phase offset.

For the complete encoder assignments, see [SEQ reference](../reference/partner.md). To understand what remains when loading scenes, see [What is saved?](../reference/scenes.md#what-a-scene-stores).
