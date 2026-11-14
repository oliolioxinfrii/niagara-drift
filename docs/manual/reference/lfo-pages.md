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
| LFO sends (2/3) | I pitch / X-Wave | I cutoff / resonance | II pitch / X-Wave | II cutoff / resonance |
| Mods (3/3) | Speed mod | Source | Delay mod | Cross-mod |

The first page displays the two LFO waveforms, not the two drone oscillator waveforms. A moving dot and a brighter trailing segment show each wave's position in the current cycle, including when Speed modulation changes the progression. Either LFO can modulate either drone, according to the routing on page 2.

### LFO waves

- Turn KNOB 1 or 3 to select a named waveform, wrapping from the last waveform to the first.
- Turn KNOB 2 or 4 to adjust that LFO's amplitude, up to 100%.
- Hold [[OCT-]] to bypass LFO I or [[OCT+]] to bypass LFO II; the selected waveform and amplitude are retained.

### LFO sends

<img class="shot" src="../../img/screen-lfo.png" width="240" alt="The LFO sends page: the two LFOs and each drone's modulation routes">

The picture uses the original 0.2-style signal-flow layout: LFO I and II and their waveform names appear on the left, with colored lines connecting them to drone I's and II's pitch/X-Wave and filter destinations on the right. A blue connection also shows the delay's LFO route; its controls remain on **Mods**, not on this page. The waveform plots and their moving markers appear on **LFO Waves** instead. Active routes appear bright, while sends at zero depth, bypassed or off waves, and routes with Movement at 0% appear subdued.

The four knobs below the picture still set the two oscillator and two filter send depths, with larger numeric readouts. The center of each send's knob is zero. Positive and negative settings select modulation depth and direction.

- **Pitch/X-Wave:** each drone's oscillator LFO route. [[G#3]] and [[G#4]] select the destination.
- **Cutoff/resonance:** each drone's filter route. [[C#4]] selects drone I and [[D#4]] selects drone II. **Cutoff is the default**, so an LFO can visibly change pitch without changing resonance until its filter destination is switched to Reso. Use KNOB 2 for drone I or KNOB 4 for drone II on LFO sends to set filter modulation depth; also check that Movement (ALGORITHM), the selected LFO level, and the filter send are nonzero. On HOME → Pitch, resonance displays its changing effective value while playing and its stored setting while the knob is being turned.
- **LFO lock/reset:** [[C#5]] immediately precedes [[D#5]] (delay LFO). Tap [[C#5]] to lock/free the LFO to Swell; hold it for 500 ms to reset LFO phase.
- The drone's corresponding black keys select LFO I, II, or both, independently of the two global LFO waveforms.

### Mods

<img class="shot" src="../../img/screen-lfo2.png" width="240" alt="The Mods page: LFO speed modulation, delay modulation and cross-modulation">

- **Speed mod:** one waveform speeds and slows the common LFO clock.
- **Source:** LFO I, LFO II or both.
- **Delay mod:** modulates the delay time.
- **Cross-mod:** audio-rate interaction between the drones; it is not part of the LFO.

## Good to know

- The shared Rate is controlled by SELECT; global Movement is controlled by ALGORITHM.
- Long OCT bypass affects whichever LFO is selected, not a particular drone.
- [[SEL]] + [[OCT-]] and [[SEL]] + [[OCT+]] fade out the individual drones; these are separate from bypassing LFO I or II.
