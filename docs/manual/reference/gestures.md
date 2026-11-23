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

**SEL + PRESETS** adjusts **Morph Time**, also found at **GLO → Setup 1 → Morph**. It controls gesture rise and return speed, STILL phase rejoining, and the SEL+OCT drone in/out transition. The separate Gesture Time behavior and SEL-to-latch gesture shortcut are retired. The screen reports a percentage rather than a separate “latched” label.

An active Motion Loop can replay original gesture amounts; the exact session behavior of STRIP/DARK/STILL/TAIL is separate from stored scene values.

### GHOST excitation events: keys 13–16

These keys excite the existing GHOST eight-resonator bank, rather than introducing four new voices. They are **events**, not persistent 0–100% gesture depths. Their repeating latches are **session-only**, not stored in scenes.

| Key | Event | Single press | Repeats while held |
|---|---|---|---|
| **13 · D5** | STRIKE | A stronger, sharply decaying impact into the shared resonators | Every 300 ms |
| **14 · E5** | SCRAPE | 250 ms of bright, rough excitation driven by interaction between the two parent waveforms | Every 420 ms |
| **15 · F5** | BOW | A smooth, roughly 1.6-second tonal excitation that follows the two parents' changing sound and pitch | Every 1600 ms |
| **16 · G5** | TREMOR | Seven irregular, alternating impacts emphasizing the lower resonators | Every 900 ms |

**Single tap:** one event fires immediately. **Hold:** the event repeats until release. **Double-tap:** two short presses (second press within approximately 320 ms after the first release) start automatic repeating even after release. **Press the same key once more:** cancel the latch and stop new events; resonators already ringing decay naturally. A long hold does not count toward a double-tap.

**PRESETS** sets the events' shared excitation level (0–100). At 50, the events have their original intensity; 0 removes their additional excitation; at 100, it increases their injected energy and temporary GHOST accent. The setting is stored with sounds and scenes, without altering the underlying non-event GHOST timbre.

Each latch operates independently, so STRIKE and TREMOR (for example) can remain running together. Latched keys pulse their LEDs; a non-latched held key illuminates steadily. GHOST MIX governs audibility: at MIX 0, GHOST is bypassed after its transition. The GHOST TAIL recovery gate also attenuates event energy while fully closed and during re-entry. SCRAPE and BOW derive their excitation from the **actual parent audio signals**, not independent noise effects, and are not intended to produce their normal excitation from a single parent.

## Combining gestures

The controls are designed to overlap. For example, hold DARK I while using STILL II to keep one drone harmonically simple while the other's time progression slows. STRIP can expose an oscillator's fundamental while TAIL allows already generated delay and reverb to ring out.

There is no requirement to combine them in prescribed patterns. See [Gesture recipes](../recipes/gestures.md), [Motion Loop](../recipes/performing.md#loop) and [What a scene stores](scenes.md#what-a-scene-stores).
