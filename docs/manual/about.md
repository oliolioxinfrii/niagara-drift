---
title: About Volcano
intro: Its sound engine, origins, technical design and credits
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## About Volcano

Volcano is a custom FM-1 drone firmware based on [Niagara Drift](https://github.com/evanyerburgh/niagara-drift) by Evan Yerburgh. Niagara Drift provides the foundation: two continually sounding drones, modulation, gestures, a responsive sound engine and effects. Volcano expands this with a programmable pitch sequencer, sixteen performance gestures, a SAVE-based scene browser and the geological interface.

The changes were inspired by Nicolas Godin's score for *Fire of Love* (2022), particularly "Acid Lake": a foreboding drone progression whose changes unfold over time. The sequencer therefore emphasizes **up to four pitches per drone with individually positioned events and adjustable transitions**, rather than a conventional evenly spaced step grid.

Volcano is intended to be one instrument in a larger arrangement, not a complete workstation. The FM-1's processor already runs the synthesis, modulation, sequencing and interface. Connect its 3.5 mm audio output or USB audio to a compatible recording setup or another instrument to build a larger composition.

## Sound and movement

- **Two drones:** continuously blended oscillator shapes, partner oscillators, subs, noise, resonant filters, drive and pitch linking.
- **Movement:** two routed LFO signals, Swell envelopes, Drift, Wander and Age; gestures can move many sound parameters at once.
- **Sequencing:** up to four pitch events per drone; independently adjustable arrival times, total duration, drone II phase and the four pitch-transition shapes.
- **Performance:** sixteen gestures (four subtractive actions per drone plus eight macros), a motion loop, 16 factory presets and 48 user scenes selected through the orbital SAVE browser.
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
| **Gesture input** | All 16 white-key gestures use hold/tap timing, not aftertouch or velocity. Keys 1–8 operate STRIP, DARK, STILL and TAIL for each drone; 9–16 are the eight original macros. PRESETS adjusts Morph Time. |
| **MIDI** | in, over USB and TRS: notes on channels 1 and 2 tune the drones; the clock (24 pulses a beat) and Start, with Sync on or off ([MIDI](reference/midi.md)) |
| **Storage** | Six banks of eight user scenes, each with an optional 11-character name and stored pitch-sequencer pattern, plus 16 immutable factory presets. A scene stores the authored sequence, but not its running transport, editor cursor, or live STILL offsets. |
| **License** | GPL-3.0-only, except for designated MPL-2.0 components; see [LICENSING.md](../../LICENSING.md) |


## Credits and lineage

### Niagara Drift and its predecessors

[Niagara Drift](https://github.com/evanyerburgh/niagara-drift) is the work
of **Evan Yerburgh**. It was named for *Day of Niagara* (Theatre of Eternal
Music, 1965) and La Monte Young's *Drift Studies*. Its musical influences
included the Stylophone CPM DS-2, Moog Subharmonicon and Soma Lyra-8.

**Volcano is a derivative of Niagara Drift.** The projects below belong
to *Niagara Drift's* development and attribution history. They are named
because Volcano inherits that foundation—not because I separately
borrowed from each predecessor.

| Niagara Drift predecessor or resource | Developer | Contribution to Niagara Drift's lineage |
|---|---|---|
| [FoMni](https://github.com/charlesvestal/fm1-fomni) and [X0X](https://github.com/charlesvestal/fm1-x0x) | Charles Vestal | Codebase, host simulator and browser emulator |
| [Felucca](https://github.com/hugelton/Felucca) | Leo Kuroshita / Hügelton Instruments | FM-1 platform, hardware interfacing, USB, storage and installer |
| [MONK-1](https://github.com/charlesvestal/fm1-monk) | Charles Vestal | Key-pressure input |
| [ChoralRoot](https://github.com/Quixotic7/ChoralRootFM1) | Quixotic7 | Delay/reverb design, scene-crossfade and recovery ideas |
| [fm1-base](https://github.com/charlesvestal/fm1-base) | Charles Vestal | Safe restore and flash-storage work (MPL-2.0) |
| [FM-1 Screen Reader](https://github.com/charlesvestal/fm1-reader) | Charles Vestal | Accessible screen announcements and reading interface |
| [Groove OS](https://www.groove-os.com/) | — | A precedent for FM-1 firmware in a browser |

The specific reuse and license requirements are documented in
[LICENSING.md](../../LICENSING.md).

### Other influences on Volcano

**Jiant** is another FM-1 firmware I use extensively. **Frasil** and
**AMB-1** were also at the forefront of my thinking, especially as points
of comparison for defining the boundaries of Volcano's intended role.
These three projects informed my perspective on what an FM-1 instrument
can do, but I **did not directly borrow code or designs from them**.

The direct musical catalyst for Volcano was Nicolas Godin's score for
*Fire of Love* (2022), particularly **"Acid Lake."** The intention was
to give Niagara Drift's evolving drones a freely spaced pitch
progression without turning the FM-1 into an all-in-one workstation.

Volcano is not affiliated with or endorsed by Evan Yerburgh, the
developers above, M-VAVE or the makers of any referenced instrument.
