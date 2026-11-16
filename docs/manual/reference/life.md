---
title: Life and Tone
intro: ENV: Feedback, Drift, Character, Age; each drone's noise
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Life and Tone

What keeps the drones alive and worn, and each drone's noise.

## Knobs

[[ENV]] opens Life; [[ENV]] again, Tone.

| Page | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|---|
| Life | feedback | drift | character | age |
| Tone | I noise | I color | II noise | II color |

## Life

> **Screenshot needed — Life observatory:** `build/scenarios/volcano-ui/life.png` and `life-lava-full.png`.

| Knob | What it does |
|---|---|
| **Feedback** | the finished sound, after the delay and reverb, fed back into the drones: a resonant, thickening color. Past about 70 it sings on its own |
| **Drift** | slow random modulation: each drone's pitch, cutoff, wave, level and partner detune wander around where you set them. 0 holds everything still |
| **Character** | the oscillators from clean to vintage: softer edges, a bent saw, a little pitch jitter |
| **Age** | wear like an old tape: darker, a little hiss, slow wow and flutter |

## Tone

> **Screenshot needed — Tone:** `build/scenarios/volcano-ui/tone.png`.

| Knob | What it does |
|---|---|
| **Noise** | how much noise beside the oscillators: 100 is level with a saw. It goes through the drone's filter |
| **Color** | white (left) to pink (right) |

## Good to know

- **Feedback** rings in key, on drone I's harmonics, and holds about the same loudness when it sings.
- **Age** wears in over about 20 s, and out as slowly. Character is the oscillators' own wear; Age is the tape's.
- **Noise** follows the cutoff, resonance, LFO and Drift, since it's in the filter.
- **VOLCANO's Life visualization** depicts Character as lava in the vent and over the flank, Feedback as plume fullness and strands, Drift as the plume's irregular movement, and Age as fine ash and weathering.
- **The Tone visualization** uses separate ash/particle fields for the two drones. Noise governs particle density; Color changes the distribution while the WHITE/PINK labels remain explicit.
