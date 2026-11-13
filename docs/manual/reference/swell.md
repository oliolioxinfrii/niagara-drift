---
title: Swell
intro: ARP: two drones breathing at one rate, with an adjustable phase offset
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Swell

A volume envelope applied independently to the two drones. The drones share one rate, shape and depth, but drone II can be shifted relative to drone I.

## Buttons and pages

- **Tap [[ARP]]** to open the Swell page; tap again for Swell shape. Further taps alternate the two pages.
- **Hold [[ARP]]** (at least 400 ms, then release) to turn Swell on or off from any page.
- The two pages keep the same Swell on/off state.

## Knobs

| Page | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|---|
| Swell (1/2) | Rate (on a MIDI clock: Division) | Depth | Phase | Swing |
| Swell shape (2/2) | Wave | Skew | Edge | Width |

### Swell (1/2)

- **Rate** sets the duration of each breath, or its musical division when following MIDI clock.
- **Depth** sets how far the level dips, from no modulation to silence.
- **Phase** offsets drone II relative to drone I from 0% (together) through 50% (alternating) to 100% (together again).
- **Swing** makes every other breath longer, up to 3:1.

### Swell shape (2/2)

- **Wave** morphs continuously from the saved original Shape through **Double**, **Steps**, and **Hold**.
- **Skew** moves the envelope's rise and release timing earlier or later.
- **Edge** rounds or sharpens transitions toward a hard gate.
- **Width** narrows or broadens the elevated part of the envelope.
- **Hold [[SEL]] and turn KNOB 1** to edit the previous Shape parameter (gate, even, ramp, or the three original special shapes). Existing scene values are retained.

Each drone uses the same Wave/Skew/Edge/Width, but its phase is evaluated separately. The Shape page shows both waveforms at their relative phases.

## Good to know

- The starting sound is a slow, gentle breath. The long ARP press bypasses it without losing the settings.
- A phase offset of 0% preserves the common-envelope behavior of earlier Niagara Drift versions.
- A locked LFO ([[D#4]], held) follows Swell's bar starts. The two drone envelopes do not have independent clocks.
- At high Swell rates, the 3 ms output glide rounds the steepest transitions.
