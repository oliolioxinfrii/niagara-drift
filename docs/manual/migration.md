---
title: From Niagara Drift
intro: The controls that changed when Niagara Drift became Volcano
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## From Niagara Drift to Volcano

Volcano retains the two continuously sounding drones, modulation, effects and many controls developed in [Niagara Drift](https://github.com/evanyerburgh/niagara-drift). This comparison is intended for players familiar with that firmware. New users can begin with [How Volcano works](index.md#how-volcano-works) and [Controls](reference/controls.md).

| Earlier Niagara Drift workflow | Current Volcano development branch |
|---|---|
| White keys 1–8 recall scenes | White keys **1–4** now provide STRIP, DARK, STILL and TAIL for drone I; **5–8** do the same for drone II |
| SEL + gesture keys choose scene banks | **SAVE** opens an orbital browser; its four knobs choose bank, slot, action and transition percentage |
| SELECT adjusts LFO rate | **SELECT** changes pages within a family; **SEL + SELECT** adjusts LFO rate |
| Pressure gestures and tap-to-latch behavior | **All 16 gestures use timed holds and taps**, independent of pressure. Releasing a held key retains its amount |
| PRESETS sets a separate Gesture Time | **PRESETS** sets **Morph Time**, also available on GLO → Setup 1 |
| C♯5 locks LFO to Swell / resets phase | **Tap C♯5** pauses/resumes LFOs; **hold** rewinds LFO and Swell smoothly to phase zero |
| SEL displays a black-key map popup | Holding **SEL** leaves the current parameter page visible and enables its modifiers |
| Scenes recall sound parameters only | User scenes store **sound and authored 1–4-step sequence**. **LOAD** is immediate; a partial TRANSITION retains the working sequence |
| Normal black-key switches on ordinary pages | On **SEQ → Sequence**, all 27 piano keys enter pitches |
| Older HOME and GLO layouts | GLO has two Setup pages; holding HOME displays the temporary geological activity atlas |

The eight original gestures remain Tick, Chord, Snarl, Stutter, Weight, Shift, Clean and Wash on white keys 9–16, but **their input behavior has changed**. STRIP, DARK, STILL and TAIL are additional per-drone controls, not reassignments of the old scene browser.

> **Screenshot needed — New white-key map:** Capture the actual 16-gesture layout from the subtractive/STILL firmware, not the original Niagara Drift panel.

This comparison describes the **unverified development branch**, not a confirmed hardware release. For working instructions, see [Gestures](reference/gestures.md), [Scenes](reference/scenes.md) and the [first-sequence tutorial](recipes/sequencing.md).
