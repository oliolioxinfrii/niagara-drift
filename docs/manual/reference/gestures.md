---
title: Gestures
intro: Sixteen hold-and-tap controls, including independent subtractive gestures
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Gestures

All **16 white keys** are performance gestures on ordinary pages. They are **independent of pressure and velocity**; the amount depends on how long you hold a key. On **SEQ → Sequence**, the keyboard instead enters pitches.

> **Screenshot needed — Sixteen gesture keys:** Capture a real v1.5 panel map identifying STRIP / DARK / STILL / TAIL for each drone and the eight remaining gestures.

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

### Sound gestures: keys 9–16

| Key | Gesture | Effect |
|---|---|---|
| **9 · G4** | Tick | Convert the low sub into clicks/flutter |
| **10 · A4** | Chord | Shift the partners into a chord in the selected scale |
| **11 · B4** | Snarl | Introduce detuning, cross-modulation, drive and feedback |
| **12 · C5** | Stutter | Increase Swell gating for rhythmic chopping |
| **13 · D5** | Weight | Increase low-end weight and sub, round the tone |
| **14 · E5** | Shift | Change the relationship between drone pitches |
| **15 · F5** | Clean | Remove grit, movement and effects while keeping the current pitches |
| **16 · G5** | Wash | Increase delay/reverb toward a sustained wash |

These eight gestures retain the established sonic transformations. Unlike STRIP/DARK/STILL/TAIL, their **held amounts can be saved as scene latch settings**.

### How every gesture key behaves

- **Hold:** increase that gesture's intensity linearly toward **100%**, at a rate set by **Morph Time**. The gesture stops increasing at 100%.
- **Release:** keep the **exact amount reached**. Releasing does *not* automatically fade it away.
- **Tap an active, stationary key:** start a linear return toward **0%**.
- **Tap during the return:** pause at the current amount; tap again to resume its return.
- **Hold an active key longer than approximately 220 ms:** increase again from the amount already reached.
- **Tap from zero:** a brief press can establish a small retained amount, rather than jumping immediately to 100%.

**PRESETS** now adjusts **Morph Time**, also found at **GLO → Setup 1 → Morph**. It controls gesture rise and return speed and the STILL phase rejoining. The previous separate Gesture Time behavior and SEL-to-latch gesture shortcut are retired. The screen reports a percentage rather than a separate “latched” label.

An active Motion Loop can replay original gesture amounts; the exact session behavior of STRIP/DARK/STILL/TAIL is separate from stored scene values.

## Combining gestures

The controls are designed to overlap. For example, hold DARK I while using STILL II to keep one drone harmonically simple while the other's time progression slows. STRIP can expose an oscillator's fundamental while TAIL allows already generated delay and reverb to ring out.

There is no requirement to combine them in prescribed patterns. See [Gesture recipes](../recipes/gestures.md), [Motion Loop](../recipes/performing.md#loop) and [What a scene stores](scenes.md#what-a-scene-stores).
