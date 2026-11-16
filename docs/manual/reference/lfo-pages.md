---
title: LFO waves, Sends and Mods
intro: LFO: waves first, then their destinations and additional modulation
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## LFO pages

Tap [[LFO]] repeatedly to cycle through three pages:

| Page | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|---|
| LFO waves (1/3) | LFO I waveform | LFO I level | LFO II waveform | LFO II level |
| LFO sends (2/3) | I pitch / X-Wave | I filter (Cutoff / Resonance / Pan) | II pitch / X-Wave | II filter (Cutoff / Resonance / Pan) |
| Mods (3/3) | Speed mod | Source | Delay mod | Cross-mod |

> **Screenshot needed — LFO Waves:** `build/scenarios/volcano-ui/deep-mantle-lfo.png`.

The first page displays the two LFO waveforms, not the two drone oscillator waveforms. A moving dot and a brighter trailing segment show each wave's position in the current cycle, including when Speed modulation or per-drone STILL changes its local progression. Waveform displays now distinguish the drone-local phases. Either LFO can modulate either drone, according to the routing on page 2.

### LFO waves

- Turn KNOB 1 or 3 to select a named waveform, wrapping from the last waveform to the first.
- Turn KNOB 2 or 4 to adjust that LFO's amplitude, up to 100%.
- Hold [[OCT-]] to bypass LFO I or [[OCT+]] to bypass LFO II; the selected waveform and amplitude are retained.

### LFO sends

> **Screenshot needed — LFO Sends:** `build/scenarios/volcano-ui/deep-mantle-routing.png`.

The Sends page retains waveform traces, their names and the drone-local phase markers, with routes connecting LFO sources to destinations. VOLCANO uses its **Deep Mantle** accent for this page rather than the older color scheme. Both LFOs can feed the same destination. A route at zero depth, a bypassed or off waveform, or Movement at 0% appears dim. The LFO Waves page remains separate, and the controls on all three pages retain their current assignments.

The **Osc / Filter / Osc / Filter** readout and center-zero depth bars appear below the routing display. The four knobs still set the two drones' oscillator and filter send depths, including X-Wave, Resonance and Pan destination options. Positive and negative values select modulation depth and direction. The delay connection is shown for context; its depth control remains on **Mods**.

- **Pitch/X-Wave:** each drone's oscillator LFO route. [[G#3]] and [[G#4]] select the destination.
- **Cutoff/Resonance/Pan:** each drone's filter route. [[C#4]] selects drone I and [[D#4]] selects drone II. **Cutoff is the default** for the filter route; it moves filter cutoff, not oscillator pitch. The independent oscillator send on KNOB 1 or 3 controls pitch or X-Wave. Select Resonance or Pan with the relevant filter-destination key if you want the filter LFO depth to affect those instead. Use KNOB 2 for drone I or KNOB 4 for drone II on LFO Sends to set filter modulation depth; also check that Movement (ALGORITHM), the selected LFO level, and the filter send are nonzero. On HOME → Pitch, resonance displays its changing effective value while playing and its stored setting while the knob is being turned.
- **LFO transport:** [[C#5]] immediately precedes [[D#5]] (delay LFO). Tap C♯5 to pause or resume both LFO waveforms at their current phase; hold it for about 500 ms to smoothly rewind the LFO and Swell progressions to zero at the LFO rate and resume. The former LFO-to-Swell phase lock is retired.
- The drone's corresponding black keys select LFO I, II, or both, independently of the two global LFO waveforms.

### Mods

> **Screenshot needed — LFO Mods:** `build/scenarios/volcano-ui/deep-mantle-modulation.png`.

- **Speed mod:** one waveform speeds and slows the common LFO clock.
- **Source:** LFO I, LFO II or both.
- **Delay mod:** modulates the delay time.
- **Cross-mod:** audio-rate interaction between the drones; it is not part of the LFO.

## Good to know

- The shared rate is controlled by **SEL + SELECT**; global Movement is controlled by ALGORITHM.
- Long OCT bypass affects whichever LFO is selected, not a particular drone.
- [[SEL]] + [[OCT-]] and [[SEL]] + [[OCT+]] fade out the individual drones; these are separate from bypassing LFO I or II.
