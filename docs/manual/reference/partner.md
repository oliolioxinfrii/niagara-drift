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

VOLCANO's pitch sequencer has **one to four note events per drone**. Rather than a fixed grid of equally spaced steps, the two drones follow their own programmed pitch events around a repeating timeline.

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

The event positions need not be equally spaced. **KNOBs 1–3** set the relative arrivals of events 2–4 (where present); **KNOB 4** changes the overall loop duration while preserving the relative positions. Drone II has its own phase offset.

> **Screenshot needed — Intervals:** Capture SEQ page 4 with a multi-event sequence and unequal timing.

## Transitions (page 5)

Each knob shapes the pitch movement across one segment of the sequence:

| KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|
| 1→2 | 2→3 | 3→4 | 4→1 |

The central setting provides a smooth sinusoidal ease; toward either extreme, the movement approaches an early or late abrupt jump. Only segments that belong to the active sequence can be adjusted.

> **Screenshot needed — Transitions:** Capture SEQ page 5 with non-default shapes for each active segment.

## Transport, randomization and storage

- **SEL + PLAY** starts, pauses or resumes the pitch progression independently of the global PLAY fade.
- **SEL + HOME** creates a randomized working sound and sequence, including transition shapes. Saved scenes are not overwritten.
- **The sequence lives in RAM.** Its programmed notes and timing are not stored in scene slots; factory preset recalls reset a previously authored multistep sequence to one step.
- **Pitch Lock** and **Pitch Link** affect how the two drones' pitches follow the programmed changes.

For a short overview of the switches, see [Black keys](black-keys.md).
