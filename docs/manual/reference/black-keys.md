---
title: Black keys
intro: Switches, and the LFO's routing on the LFO pages
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Black keys

The 11 black keys are switches, with two sets of jobs: one for the drones, on every page but the LFO pages, and one
for the LFO, while an LFO page is open. **[[C#5]] handles LFO lock/reset on every page.** Outside the LFO pages, [[D#4]] links drone II's pitch to drone I; on the LFO pages, it selects drone II's filter destination. Cross-mod ([[F#5]]) is the same in both layouts.

## Drone keys

On every other page, each drone has three keys, and five more set the links between them.

| Key | Switches |
|---|---|
| [[F#3]] · [[F#4]] | drone I's · drone II's partner, on or off |
| [[G#3]] · [[G#4]] | the sub: −1 oct, −2 oct, ⅓, ⅕, −3 oct, −4 oct |
| [[A#3]] · [[A#4]] | the filter: low-, band- or high-pass |
| [[C#4]] | pitch lock: scenes and Presets keep the drones' pitches (on to start) |
| [[D#4]] | pitch link: drone II follows drone I |
| [[C#5]] | tap to lock/free the LFO to Swell; hold 500 ms to reset its phase |
| [[D#5]] | hard sync: drone II to drone I |
| [[F#5]] | cross-mod: off, I moves II's cutoff, bend each other, Snarl |

## LFO keys

On the [LFO pages](lfo-pages.md), each drone's four keys set its two sends.

<img src="../../img/black-keys-lfo.png" width="720" alt="On LFO pages C#4 switches drone I's cutoff to resonance, D#4 switches drone II's; C#5 is LFO Swell lock/reset, D#5 is Delay LFO and F#5 is cross-mod">

| Key | Switches |
|---|---|
| [[F#3]] · [[F#4]] | which LFO moves the pitch: I, II or both |
| [[G#3]] · [[G#4]] | the pitch send moves X-Wave instead |
| [[A#3]] · [[A#4]] | which LFO moves the cutoff: I, II or both |
| [[C#4]] · [[D#4]] | drone I's · drone II's cutoff send moves resonance instead |
| [[C#5]] | tap to lock/free the LFO to Swell; hold 500 ms to reset its phase |
| [[D#5]] | which LFO moves the delay's time: I, II or both |

## Finding a key while you play

<img class="shot" src="../../img/screen-keymap.png" width="240" alt="The black keys' map while SEL is held: the keys lit as on the panel, their groups named, the key touched named with its setting">

### Hold SEL

The screen draws the black keys, lit as on the panel.

### Touch a key

While SEL is held, the screen names the key and its setting, without switching it.

## Good to know

- **Lights:** dark is off, lit is on. A key of several choices is dim on its first, lit on the second, winking on a
  third.
- **Filter:** low-pass (dim), band-pass (winking), high-pass (lit).
- **Sub:** −1 octave (dim), −2 octaves (lit), then the subharmonics ⅓ and ⅕ (winking).
- **An LFO key:** LFO I (dim), II (lit), both (winking). The screen says what now moves what ("LFO II → I PITCH").
- **Pitch lock:** the pitch knobs still tune.
- **Pitch link:** KNOB 3 on Pitch sets the gap ("+7 st").
- **LFO lock/reset:** a short tap toggles Swell synchronization; a half-second hold restarts the LFO without changing synchronization.
- **A key the screen names in a box** ("needs [F#3]") blinks while the message shows.
