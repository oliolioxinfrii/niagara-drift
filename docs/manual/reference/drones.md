---
title: Drones and Pitch
intro: HOME: each drone's level, cutoff, pitch and resonance
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Drones and Pitch

The page you play from: each drone's level and cutoff; then its pitch and resonance.

## Knobs

[[HOME]] opens Drones; [[HOME]] again, Pitch. KNOB 1–2 are drone I, KNOB 3–4 drone II.

| Page | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|---|
| Drones | I level | I cutoff | II level | II cutoff |
| Pitch | I pitch | I resonance | II pitch | II resonance |

## Drones

> **Screenshot needed — HOME Drones:** `build/scenarios/volcano-ui/surface.png`.

| Knob | What it does |
|---|---|
| **Level** | how loud each drone is. Halfway is about a quarter as loud |
| **Cutoff** | opens or closes each drone's filter |

### Filter type

Low-pass (LP), band-pass (BP) or high-pass (HP), on a black key: [[A#3]] for drone I, [[A#4]] for drone II.

> **Screenshot needed — Filter switches:** A v1.5 black-key diagram highlighting A#3 and A#4.

## Pitch

> **Screenshot needed — HOME Pitch:** `build/scenarios/volcano-ui/pitch-scope.png`, plus `pitch-resonance.png` with a resonance knob selected.

| Knob | What it does |
|---|---|
| **Pitch** | 1.6 kHz down to 2 Hz, a note of the Tuning a click. With [[SEL]] held, a cent a click |
| **Resonance** | boosts the filter at its cutoff. The last dot is red: there it rings on its own (self-oscillates) |

### Pitch link

[[D#4]] links drone II to drone I outside the LFO pages: drone II's pitch knob then sets the interval between them. On LFO pages, [[D#4]] switches drone II's filter-LFO destination instead.

## Good to know

- **Under 27 Hz** a drone becomes a pulse of clicks; the speaker can't play that low, so listen on headphones.
- **Pitch glides** from note to note, so a steady turn is one slide.
- **A fine offset stays** (with SEL, +3 cents stays +3 from note to note) until you change the Tuning; "fine +3" shows
  it.
- **VOLCANO's Drones display** shows fixed geological strata, with drone I's level shaping lava and drone II's level affecting smoke. STRIP/DARK change the source sound rather than providing new oscillator modes; STILL controls that drone's modulation and pitch progression, and TAIL changes the drone's contribution to the shared audio bus. Its two scope traces and overlaid filter responses use the actual sound and filter settings.
- **The Pitch display** shows an oscilloscope-like pitch history. While adjusting resonance, the display exposes its filter-response curves.
- **Hold HOME for 400 ms** from another page to inspect the activity atlas; release HOME to return. See [Controls](controls.md).

> **Screenshot needed — Geological atlas:** `build/scenarios/volcano-ui/geological-atlas.png`.

For the new per-drone controls, see [Subtractive gestures](gestures.md).
