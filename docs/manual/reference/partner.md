---
title: Partner, Pitch moves and Sequence
intro: The five SEQ pages: oscillators, pitch movement and programming
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## SEQ: partners and pitch sequencing

**[[SEQ]]** cycles through five pages: **Partner**, **Pitch moves**, **Sequence**, **Intervals** and **Transitions**. Turning SELECT also moves among those five pages.

## Partner (page 1)

| KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|
| Drone I interval | Drone I detune | Drone II interval | Drone II detune |

Each drone has a second oscillator at a selected musical interval. Detune introduces slow beating. The partner black keys ([[F#3]] and [[F#4]]) turn those oscillators on or off.

> **Screenshot needed — Partner:** `build/scenarios/volcano-ui/crust-partner.png`.

## Pitch moves (page 2)

| KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|
| Shift's move | Pitch glide | Drone II Wander | Scale |

**Shift's move** determines where the Shift gesture moves the pitches; **Glide** sets how long pitch changes take. **Wander** moves drone II among notes on the selected scale. At zero, Wander is off.

> **Screenshot needed — Pitch moves:** Capture the second SEQ page from the current firmware.

## Sequence (page 3)

VOLCANO's pitch sequencer moves the **continuously sounding** drones between pitches; it does not retrigger notes like a drum-machine grid. There are **one to four event positions in a shared timeline**. Each drone may have its own pitch at those positions, and drone II can have a different phase offset when Pitch Lock and Pitch Link are off. The time between events is editable: a three-note sequence could move after 3 seconds, 6 seconds and 4 seconds rather than every four beats.

**A three-note example:** Program A3 → C4 → E4 on Sequence, place their arrivals at 0, 3 and 9 seconds within a 13-second cycle on Intervals, then shape the three moves on Transitions. [Follow the complete first-sequence tutorial](../recipes/sequencing.md#first-sequence) before using the parameter table as a quick reference.

| Control | Action |
|---|---|
| **KNOB 1** | Select the event being edited |
| **SEL + KNOB 1** | Retune the selected event on the current tuning grid |
| **KNOB 2** | Choose drone I or II |
| **KNOB 3** | Set the number of active events (1–4) |
| **KNOB 4** | Offset drone II's phase relative to drone I |

Press a keyboard key on this page to enter and audition a pitch. **All physical keys are note inputs here**: they do not trigger ordinary black-key switches, scene selection or gestures.

> **Screenshot needed — Sequence:** `build/scenarios/volcano-ui/crust-sequence.png`.

## Intervals (page 4)

The event positions need not be equally spaced. **KNOBs 1–3** move events 2–4 (where present) without moving their neighbors; **KNOB 4** changes the overall loop duration while preserving the events' relative positions. Both drones use this spacing pattern. Drone II can have a phase offset within the same cycle.

> **Screenshot needed — Intervals:** Capture SEQ page 4 with a multi-event sequence and unequal timing.

## Transitions (page 5)

Each knob shapes the pitch movement across one segment of the sequence:

| KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|
| Segment 1 (1→2) | Segment 2 (2→3, or 2→1 with two steps) | Segment 3 (3→4, or 3→1 with three steps) | Segment 4 (4→1, with four steps) |

The final active segment always returns to **step 1**. With two steps, only KNOBs 1–2 are active; with three steps, only KNOBs 1–3 are active. At one step, transition-shape editing is unavailable. The central setting provides a smooth sinusoidal ease; at −100 the pitch changes at the *end* of the segment, and at +100 it changes at the *beginning*.

**Step 1** follows the current pitches on **HOME → Pitch**. Changing those live pitches changes the first sequencer event, and Pitch Lock or Pitch Link can make drone II follow drone I instead of an independent sequence.

> **Screenshot needed — Transitions:** Capture SEQ page 5 with non-default shapes for each active segment.

## Transport, randomization and storage

- **SEL + PLAY** starts, pauses or resumes the pitch progression independently of the global PLAY fade.
- **SEL + HOME** creates a randomized working sound and sequence, including transition shapes. Saved scenes are not overwritten.
- **The sequence lives in RAM.** Its programmed notes, timing, phase and transition shapes are not stored in scene slots; factory preset recalls reset a previously authored multistep sequence to one step. [What a scene stores](scenes.md#what-a-scene-stores) explains the distinction.
- **Pitch Lock** and **Pitch Link** affect how the two drones' pitches follow the programmed changes.

For a short overview of the switches, see [Black keys](black-keys.md).
