---
title: About Volcano
intro: Its sound engine, origins, technical design and credits
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## About Volcano

Volcano is a custom FM-1 drone firmware based on [Niagara Drift](https://github.com/evanyerburgh/niagara-drift) by Evan Yerburgh. Niagara Drift provides the foundation: two continually sounding drones, modulation, gestures, a responsive sound engine and effects. Volcano expands this with a programmable pitch sequencer, a SAVE-based scene browser and the geological interface.

The changes were inspired by Nicolas Godin's score for *Fire of Love* (2022), particularly "Acid Lake": a foreboding drone progression whose changes unfold over time. The sequencer therefore emphasizes **up to four pitches per drone with individually positioned events and adjustable transitions**, rather than a conventional evenly spaced step grid.

Volcano is intended to be one instrument in a larger arrangement, not a complete workstation. The FM-1's processor already runs the synthesis, modulation, sequencing and interface. Connect its 3.5 mm audio output or USB audio to a compatible recording setup or another instrument to build a larger composition.

## Sound and movement

- **Two drones:** continuously blended oscillator shapes, partner oscillators, subs, noise, resonant filters, drive and pitch linking.
- **Movement:** two routed LFO signals, Swell envelopes, Drift, Wander and Age; gestures can move many sound parameters at once.
- **Sequencing:** up to four pitch events per drone; independently adjustable arrival times, total duration, drone II phase and the four pitch-transition shapes.
- **Performance:** eight gestures, a motion loop, 16 presets and 48 user scenes selected through the orbital SAVE browser.
- **Effects:** delay, reverb, post-delay drive, feedback and stereo controls.
- **Display:** VOLCANO's five geological layer identities show sound and modulation, while SAVE and GLO use distinct orbital and planetary views.

For editing steps, see [Controls](reference/controls.md), [SEQ](reference/partner.md) and [Scenes](reference/scenes.md).

> **Screenshots needed — VOLCANO overview:** Capture the HOME Drones, geological atlas, SEQ Sequence, Orbit SAVE browser and GLO planet views using `tests/scenarios/volcano-ui.omni`. Do not use the previous Niagara Drift display images.

## Technical details

| | |
|---|---|
| **Audio** | 44.1 kHz stereo; 32-bit floating point inside, 24-bit out; a block every 5.8 ms |
| **Oscillators** | virtual analog, 4 shapes blended continuously (sine, triangle, saw, square) with pulse width; saw and pulse band-limited (polyBLEP); hard sync; cross-FM |
| **Partner** | a second oscillator on each drone: an octave or a fifth or fourth below, unison, a third, fourth or fifth above, an octave, or an octave and a fifth; each tempered or just (pure, beatless); detune up to 30 cents for slow beating |
| **Sub and noise** | a sub of 4 waves (square, sine, triangle, saw), 1–4 octaves down or the subharmonics ⅓ and ⅕; under a low drone the lowest become ticks and pulses ([recipe](recipes/sounds.md#ticks)); noise from white to pink |
| **Filter** | a state-variable filter (zero-delay feedback, after Andrew Simper): low-, band- or high-pass, resonant to self-oscillation; an antialiased drive |
| **LFO** | 2 waves of 10 shapes each (20 in all: triangles, squares, saws, exponentials, patterns, random, AND and OR steps), one cycle in 100 s to 70 Hz, on pitch, wave, cutoff, resonance and the delay |
| **Tuning** | A from 427 to 453 Hz (440 to start, 432 for those who like it), 1 Hz a click or 0.1 with SEL; 12-tone, quarter tones, 19-, 31- and 53-tone, just, the harmonics, LMY, Harrison, the shrutis, Partch |
| **Detuning** | each drone a cent at a time off its Tuning's notes (SEL + a pitch knob), kept as you move between notes; each partner up to 30 cents, for beating; Drift lets the detune wander |
| **Pressure sensing** | Keys 9–12 use key pressure to control gestures; ordinary white keys 1–8 are not scene-pressure crossfade controls in Volcano |
| **MIDI** | in, over USB and TRS: notes on channels 1 and 2 tune the drones; the clock (24 pulses a beat) and Start, with Sync on or off ([MIDI](reference/midi.md)) |
| **Storage** | Six banks of eight user scenes, each with an optional 11-character name, plus 16 immutable factory presets; scene selection uses the SAVE browser and the pitch sequencer remains RAM-only |
| **License** | GPL-3.0-only, except for designated MPL-2.0 components; see [LICENSING.md](../../LICENSING.md) |


## Credits and lineage

**Niagara Drift** was created by **Evan Yerburgh** and was named for *Day of Niagara* (Theatre of Eternal Music, 1965) and La Monte Young's *Drift Studies*. Niagara Drift drew inspiration from the Stylophone CPM DS-2, Moog Subharmonicon and Soma Lyra-8. Volcano is a separate, custom derivative of that work, not an official upstream release.

| From | By | What it gave |
|---|---|---|
| [FoMni](https://github.com/charlesvestal/fm1-fomni) and [X0X](https://github.com/charlesvestal/fm1-x0x) | Charles Vestal | the code it started from, the host simulator, the browser emulator |
| [Felucca](https://github.com/hugelton/Felucca) | Leo Kuroshita, Hügelton Instruments | the FM-1 platform: hardware layer, USB, update loader, storage, installer |
| [MONK-1](https://github.com/charlesvestal/fm1-monk) | Charles Vestal | reading the keys' pressure |
| [ChoralRoot](https://github.com/Quixotic7/ChoralRootFM1) | Quixotic7 | the delay and reverb's design; the original scene and backup-before-install ideas |
| [fm1-base](https://github.com/charlesvestal/fm1-base) | Charles Vestal | the safe restore's flash core (MPL-2.0) |
| [FM-1 Screen Reader](https://github.com/charlesvestal/fm1-reader) | Charles Vestal | the screen's words over USB and the page that speaks them (its firmware side from fm1-base, MPL-2.0); thanks to João Durães (WoodJack) for asking for it |
| [Groove OS](https://www.groove-os.com/) | | the idea of running FM-1 firmware in a browser |


The original Niagara Drift project and its contributors deserve explicit credit for the engine, interface groundwork, audio effects and FM-1 platform. The changes in this repository do not imply endorsement by those contributors, M-VAVE or the makers of the instruments mentioned above.

Licenses and attribution are documented in [LICENSING.md](../../LICENSING.md).
