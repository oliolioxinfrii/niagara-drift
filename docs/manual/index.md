---
title: Volcano manual
intro: A guide to the sound, controls, performance and setup
description: Volcano firmware for the M-VAVE FM-1; based on Niagara Drift by Evan Yerburgh.
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

# Volcano manual

Volcano is a two-drone synthesizer for the M-VAVE FM-1, built on [Niagara Drift](https://github.com/evanyerburgh/niagara-drift). The FM-1's ordinary firmware is replaced with a continuously sounding instrument that can evolve through gestures, modulation, pitch sequences and effects.

## How Volcano works

**Two drones sound continuously.** Each has its own oscillators, sub, noise, filter and pitch; the ordinary **HOME**, **EDIT** and **ENV** pages shape these sounds. **FX** adds delay and reverb.

**Several kinds of movement can coexist.** The two LFO waveforms share one adjustable clock and can be routed to different destinations. **Swell** shapes amplitude in recurring breaths. **Drift**, **Wander** and **Age** introduce other changes. The **SEQ** pitch sequencer moves the drones through up to four pitches without retriggering them. **Gestures** temporarily reshape multiple parameters, while **REC** can record and repeat performance movements. These are complementary tools, not a single rhythm grid; the combinations are left for you to explore.

**Different timescales can make a very long cycle.** In an *idealized* example, suppose Swell repeats every **7 seconds**, the shared LFO cycle every **11 seconds**, a pitch sequence every **13 seconds**, and a motion loop every **17 seconds**. If their clocks are independent, exact and uninterrupted, their phases next all coincide after **7 × 11 × 13 × 17 = 17,017 seconds (4 h 43 min 37 s)**: their least common multiple. This is an illustration, not a promise of exact recurrence in the firmware. LFO I and II share their clock, synchronization can couple clocks, and random movement such as Drift or Wander may never repeat exactly.

**Scenes preserve sounds, not pitch sequences.** Press **SAVE** to browse factory presets and 48 user scene slots, or to load, rename or transition between sounds. A scene holds synthesis settings and the selected Tuning. Your programmed sequencer notes and event times are **RAM-only** and do not go into the scene slot; Fade, Morph, Scene time, Stereo and Speaker belong to the instrument's separate settings.

## Where to begin

For a first session, [build a sound from Bare](recipes/scratch.md), then [program an uneven three-note sequence](recipes/sequencing.md#first-sequence). To keep a sound, follow [Scenes and saving](recipes/scenes.md); for control lookup, use the [Reference](reference/index.md). Players coming from older Niagara Drift versions can consult [From Niagara Drift](migration.md).

<ul class="doors">
<li><a href="recipes/index.md"><img class="ic" src="../../web/manual/icons/cooking-pot.svg" alt=""><small>Make a sound</small><b>Recipes</b><span>Build sounds, program movement, use gestures and transitions</span></a></li>
<li><a href="reference/index.md"><img class="ic" src="../../web/manual/icons/book-open.svg" alt=""><small>Find a control</small><b>Reference</b><span>Every page, knob, switch, scene and sequencer control</span></a></li>
<li><a href="reference/shortcuts.md"><img class="ic" src="../../web/manual/icons/keyboard.svg" alt=""><small>Look it up</small><b>Shortcuts</b><span>Button combinations and hold actions</span></a></li>
<li><a href="help.md"><img class="ic" src="../../web/manual/icons/lifebuoy.svg" alt=""><small>Install or recover</small><b>Help</b><span>Build, install, back up and restore the FM-1</span></a></li>
</ul>

> **Screenshots in preparation:** Original Niagara Drift screenshots cannot represent the Volcano interface accurately. The reference and recipes contain named placeholders for captures from the v1.5 host simulator; those placeholders will be replaced with verified images.

For technical background and attribution, see [About Volcano](about.md). Source builds and emulator instructions are in [BUILDING.md](../../BUILDING.md).
