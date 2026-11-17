---
title: Build a sound from scratch
intro: Start with the Bare preset and add one element at a time
description: A step-by-step Volcano v1.5 tutorial using the SAVE browser and sound-shaping controls.
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

### Build a sound from scratch {#scratch}

*Begin with one oscillator and gradually introduce the second drone, sub, modulation and effects.*

1. Tap **SAVE**. Choose **Presets 9–16** with KNOB 1, slot **8** with KNOB 2 (**P16 Bare**), and **LOAD** with KNOB 3. Press SAVE to load it. Press PLAY if the global sound is stopped.
2. Tap **HOME** until **Drones** appears. Turn KNOB 2 to lower drone I's filter cutoff until the sawtooth becomes darker.
3. Use **SEL + OCT+** to bring drone II in. Tap HOME again to reach **Pitch** and turn KNOB 3 to tune it relative to drone I.
4. Tap **EDIT** until **Sub · Drive** appears. Turn KNOB 1 to blend drone I toward its sub oscillator.
5. Raise **ALGORITHM** (Movement) from zero. Movement scales the strength of routed LFO signals; by itself it makes no audible change if all send depths are zero.
6. Open **LFO → Waves** and select an active waveform (for example Triangle) and nonzero level for **LFO I**. Then open **LFO → Sends**. On this page, [[A#3]] selects the LFO source for drone I's filter send, and [[C#4]] selects its destination; use them until the display shows **LFO I** and **Cutoff**. Raise **KNOB 2** from zero to hear filter movement. If the send is silent, check the LFO level, waveform, Movement, and routing in that order.
7. Tap **FX** for **Space**. Turn KNOB 2 for delay mix and KNOB 4 for reverb mix. Explore **ENV → Life** for Drift and Character.
8. To keep the result, open SAVE, select a user bank (A–F), an empty slot, and the SAVE action; press SAVE to store the sound. If you also program a pitch sequence, it will be saved in that slot.

> **Screenshot needed — Starting preset:** Capture P16 Bare selected in the v1.5 Orbit browser.

> **Screenshot needed — Finished patch:** Capture a representative v1.5 Drones display after adding sub, modulation and effects.

Scenes preserve both this sound and an authored pitch sequence. Next, try [First pitch sequence](sequencing.md) to put the drones in motion through a series of notes, then save the complete idea.

For more detail, see [Scenes and saving](scenes.md) and the [Drones](../reference/drones.md), [LFO](../reference/lfo-pages.md) and [FX](../reference/space.md) reference pages.
