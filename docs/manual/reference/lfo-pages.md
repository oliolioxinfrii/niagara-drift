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

The first page displays the two LFO waveforms, not the two drone oscillator waveforms. Either LFO can modulate either drone, according to the routing on page 2.

### LFO waves

- Turn KNOB 1 or 3 to select a named waveform, wrapping from the last waveform to the first.
- Turn KNOB 2 or 4 to adjust that LFO's amplitude, up to 100%.
- Hold [[OCT-]] to bypass LFO I or [[OCT+]] to bypass LFO II; the selected waveform and amplitude are retained.

### LFO sends

<img class="shot" src="../../img/screen-lfo.png" width="240" alt="The LFO sends page: the two LFOs and each drone's modulation routes">

The center of each send's knob is zero. Positive and negative settings select modulation depth and direction.

- **Pitch/X-Wave:** each drone's oscillator LFO route. [[G#3]] and [[G#4]] select the destination.
- **Cutoff/resonance:** each drone's filter route. [[C#4]] and [[C#5]] select the destination.
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
