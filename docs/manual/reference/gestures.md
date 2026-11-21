---
title: Gestures
intro: Twelve held-depth gestures and four resonator excitation keys
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Gestures

On ordinary pages, white keys **1–12** control retained gesture depths; keys **13–16** trigger percussive GHOST excitation events. The first twelve are independent of pressure and velocity: their amount depends on how long a key is held. On **SEQ → Sequence**, all keys enter pitches instead.

> **Screenshot needed — Performance key map:** Capture an authentic panel map identifying keys 1–8 as STRIP/DARK/STILL/TAIL, keys 9–12 as Tick/Chord/Snarl/Stutter, and keys 13–16 as GHOST STRIKE/SCRAPE/BOW/TREMOR.

### Subtractive gestures: keys 1–8

| Key | Drone | Action |
|---|---|---|
| **1** | I | **STRIP** — progressively remove partner, sub and noise, leaving the main oscillator |
| **2** | I | **DARK** — move the oscillator's effective X-Wave toward sine and reduce drive/Character harmonics |
| **3** | I | **STILL** — slow its LFO, Swell and pitch-sequence progression without stopping audio |
| **4** | I | **TAIL** — lower its contribution to the shared dry/effects bus; existing echoes and reverb tails continue |
| **5** | II | **STRIP** |
| **6** | II | **DARK** |
| **7** | II | **STILL** |
| **8** | II | **TAIL** |

These are **subtractive**, not alternate oscillator or filter modes. At 100% STILL, the selected drone's **LFO, Swell and sequencer local clocks** stop advancing. Its oscillators, existing effects, Drift, Wander and Age are not frozen. The other drone can continue at its own rate. On release of STILL back toward zero, the local clocks rejoin the shared master phase smoothly over Morph Time; the correction may briefly run faster to catch up.

The two drones share the effect buffers. TAIL can stop adding new dry/effect signal from one drone but cannot isolate or remove the sound already in those buffers. These four gesture amounts per drone are **not stored in scenes** on this development branch.

### Sound gestures: keys 9–12

| Key | Gesture | Effect |
|---|---|---|
| **9 · G4** | Tick | Convert the low sub into clicks/flutter |
| **10 · A4** | Chord | Shift the partners into a chord in the selected scale |
| **11 · B4** | Snarl | Introduce detuning, cross-modulation, drive and feedback |
| **12 · C5** | Stutter | Increase Swell gating for rhythmic chopping |

These four retain their established sonic transformations. Unlike STRIP/DARK/STILL/TAIL, their **held amounts can be saved as scene latch settings**. The retired Weight/Shift/Clean/Wash key bindings are not assigned to physical white keys on this experiment branch, although legacy saved parameter values remain readable.

### How gesture keys 1–12 behave

- **Hold:** increase that gesture's intensity linearly toward **100%**, at a rate set by **Morph Time**. The gesture stops increasing at 100%.
- **Release:** keep the **exact amount reached**. Releasing does *not* automatically fade it away.
- **Tap an active, stationary key:** start a linear return toward **0%**.
- **Tap during the return:** pause at the current amount; tap again to resume its return.
- **Hold an active key longer than approximately 220 ms:** increase again from the amount already reached.
- **Tap from zero:** a brief press can establish a small retained amount, rather than jumping immediately to 100%.

**PRESETS** adjusts **Morph Time**, also found at **GLO → Setup 1 → Morph**. It controls gesture rise and return speed, STILL phase rejoining, and the SEL+OCT drone in/out transition. The separate Gesture Time behavior and SEL-to-latch gesture shortcut are retired. The screen reports a percentage rather than a separate “latched” label.

An active Motion Loop can replay original gesture amounts; the exact session behavior of STRIP/DARK/STILL/TAIL is separate from stored scene values.

### GHOST excitation events: keys 13–16

These keys excite the existing GHOST eight-resonator bank, rather than introducing four new voices. They are **events**, not persistent 0–100% gesture depths; they are not stored as latches in scenes.

| Key | Event | Single press | Repeats while held |
|---|---|---|---|
| **13 · D5** | STRIKE | Short, sharply decaying resonator excitation | Every 300 ms |
| **14 · E5** | SCRAPE | Roughly 250 ms of irregular noise excitation | Every 650 ms |
| **15 · F5** | BOW | Smooth, roughly 1.6 s sustained excitation | Every 1600 ms |
| **16 · G5** | TREMOR | Five irregular pulses biased toward the lower four resonators | Every 900 ms |

**Tap** triggers once. **Hold** retriggers at the indicated interval. **Release** stops further triggers but the resonances already excited may continue ringing. Simultaneous events share bounded injection levels. GHOST MIX governs audibility: at MIX 0, GHOST is bypassed after its transition. The GHOST TAIL recovery gate also attenuates event energy while fully closed and during re-entry.

## Combining gestures

The controls are designed to overlap. For example, hold DARK I while using STILL II to keep one drone harmonically simple while the other's time progression slows. STRIP can expose an oscillator's fundamental while TAIL allows already generated delay and reverb to ring out.

There is no requirement to combine them in prescribed patterns. See [Gesture recipes](../recipes/gestures.md), [Motion Loop](../recipes/performing.md#loop) and [What a scene stores](scenes.md#what-a-scene-stores).
