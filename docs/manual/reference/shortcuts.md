---
title: Shortcuts
intro: Page controls, transport, gesture taps and SEL combinations
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Shortcuts

### SEL combinations

| Input | Action |
|---|---|
| Hold **[[SEL]]**, turn **SELECT** | Change the shared LFO rate or clock-synchronized division |
| **SEL + PLAY** | Start/pause/resume pitch-sequencer transport without changing the global PLAY fade |
| **SEL + HOME** | Randomize the current sound and working sequence without overwriting saved slots |
| **SEL + SAVE** | Undo/redo a supported sound change |
| **SEL + OCT− / OCT+** | Toggle drone I / II on or off |
| Hold **SEL**, turn a pitch or fine-adjustable knob | Make fine adjustments or use that page's secondary control |
| **SEL + KNOB 1** on Sequence | Retune selected event on the current grid |
| **SEL + KNOB 1 or 3** on Sub · Drive | Change the selected drone's sub waveform |
| **SEL + KNOB 1** on Swell shape | Adjust original Shape |
| **SEL + KNOB 2** on Details | Change delay type |
| **SEL + KNOB 4** on Details | Change reverb input |

Holding **SEL** leaves the current parameter page visible rather than opening the old key-map overlay. **SEL is not needed to retain a gesture amount**: releasing a gesture key retains it.

### Gesture keys and navigation

| Input | Action |
|---|---|
| **White keys 1–4** | STRIP, DARK, STILL, TAIL for drone I |
| **White keys 5–8** | STRIP, DARK, STILL, TAIL for drone II |
| **White keys 9–16** | Tick, Chord, Snarl, Stutter, Weight, Shift, Clean, Wash |
| Hold any gesture key | Increase intensity toward 100% over Morph Time |
| Release a gesture key | Retain the reached intensity |
| Tap an already active gesture | Return toward zero; tap to pause or resume that return |
| Turn **PRESETS** | Adjust **Morph Time** for gesture movement |
| Tap a page button twice / turn SELECT | Cycle pages within its family |
| Hold **HOME** (~400 ms) | Inspect geological atlas; release to return |
| Hold **ARP** (~400 ms) | Toggle Swell on or off |
| Tap **SAVE** | Open the orbital scene browser |
| **SAVE** in the browser | Confirm SAVE, immediate LOAD, or timed TRANSITION |
| **HOME** in the browser | Back/cancel |
| **REC** in the browser | Rename a user slot |
| Tap **OCT− / OCT+** | Advance LFO I / II waveform |
| Hold **OCT− / OCT+** (~400 ms) | Bypass or restore that LFO |
| Tap **C♯5** outside SEQ note entry | Pause/resume both LFO waveforms at their current phase |
| Hold **C♯5** (~500 ms) outside SEQ note entry | Smoothly rewind both LFO and Swell phase progressions to zero at the current LFO rate, then resume automatically |
| Tap **REC** repeatedly outside the browser | Record, play back, stop a motion loop |
| Hold **OCT− and OCT+** for five seconds | Enter chip update/recovery mode |

On **SEQ → Sequence**, **all 27 physical piano keys** are note inputs, not ordinary switches or gestures. The normal black keys control drone switches; on the LFO pages they control modulation routing, except for shared transport and other global switches.

See [Gestures](gestures.md), [Scenes](scenes.md) and [First pitch sequence](../recipes/sequencing.md).
