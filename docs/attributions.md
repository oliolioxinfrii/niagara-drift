<!-- SPDX-License-Identifier: GPL-3.0-only -->
# Attributions, technical precedents and artistic influences

This account applies to **Volcano / Niagara Drift**, including the
`experiment/ghost14-ghost-fx-cpu1-headroom` (architecture C) firmware.
It separates **actual code provenance**, **independently reimplemented
technical methods**, **musical and instrument design references**, and
**ideas discussed but not yet implemented**. Acknowledging a product
or publication does not mean its source code, circuit or algorithms
were copied.

## Direct software ancestry and licensed components

**Niagara Drift** by [Evan Yerburgh](https://github.com/evanyerburgh/niagara-drift)
is Volcano's immediate foundation. The two-drone architecture, synthesizer
and effects, controls and modulation predate Volcano. Niagara Drift in
turn descends from the following FM-1 projects. This lineage is inherited,
not separate Volcano-specific source borrowing:

- [FoMni](https://github.com/charlesvestal/fm1-fomni) and
  [X0X](https://github.com/charlesvestal/fm1-x0x), Charles Vestal:
  application/platform and host/emulation lineage.
- [Felucca](https://github.com/hugelton/Felucca), Leo Kuroshita /
  Hügelton Instruments: the FM-1 hardware and build/platform ecosystem.
- [MONK-1](https://github.com/charlesvestal/fm1-monk), Charles Vestal,
  with [Velo](https://github.com/charlesvestal/fm1-velocity):
  key-pressure and input-sensing lineage.
- [ChoralRoot](https://github.com/Quixotic7/ChoralRootFM1), Quixotic7:
  inspiration and technical structure for the stereo delay and
  eight-line feedback-delay-network reverb, reworked in float in Niagara Drift.
- [fm1-base](https://github.com/charlesvestal/fm1-base) and
  [fm1-reader](https://github.com/charlesvestal/fm1-reader),
  Charles Vestal: safe restore and accessibility/announcement components.

The **architecture C / mode-14 CPU1 experiments** additionally adapt
the guarded secondary-core worker, mailbox/startup, retirement and
fault-handling approach from
[Arctic Scream](https://github.com/PolarPatch/Arctic-scream-FM-1)
and [felucca-bsp](https://github.com/hugelton/felucca-bsp), with
earlier work from **Melodee**. These are actual software-provenance
credits; the imported components retain the relevant MPL-2.0 notices.
See [LICENSING.md](../LICENSING.md) for the file-level licensing record.

## DSP literature and established methods

The following sources establish technical provenance and context for
methods used by the inherited engine and Volcano experiments. They are
**technical references, not claims of verbatim implementation**.

- **Andrew Simper**, *SvfLinearTrapOptimised2* and the trapezoidally
  integrated/state-variable-filter approach; also
  **Vadim Zavalishin**, [*The Art of VA Filter Design*](https://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.1.0.pdf).
  The parent voices use a topology-preserving, zero-delay-feedback SVF
  with a bounded nonlinear resonance stage.
- **Julian D. Parker, Vadim Zavalishin and Efflam Le Bivic**,
  [“Reducing the Aliasing of Nonlinear Waveshaping Using
  Continuous-Time Convolution” (DAFx, 2016)](https://www.dafx.de/paper-archive/details/vem_XXF5qBbfiWOH2RVVAA).
  The drive uses first-order antiderivative antialiasing; this is a
  published DSP technique rather than an original theoretical invention.
- **PolyBLEP** (polynomial band-limited step) oscillator correction
  is an established digital-oscillator technique used by Niagara
  Drift for saw/pulse discontinuities. No single publication is
  represented here as the source of this project's specific code.
- **John Stautner and Miller Puckette**,
  [“Designing Multi-Channel Reverberators” (1982)](https://www.ee.columbia.edu/~dpwe/e4896/papers/StautP82-reverb.pdf);
  **Jean-Marc Jot and Antoine Chaigne**,
  [“Digital Delay Networks for Designing Artificial Reverberators”
  (AES, 1991)](https://aes.org/publications/elibrary/elibrary-page/?id=5663).
  These are important feedback-delay-network precedents. The project's
  concrete reverb lineage runs through ChoralRoot as recorded above;
  the literature is not presented as direct source-code provenance.
- **Nonlinear intermodulation, combination tones, damped modal
  resonators and excitation/resonance synthesis** are established
  ideas that motivate the GHOST experiments. Mode 14 forms a nonlinear
  two-input terrain, removes one-parent terms and excites eight
  damped resonators tuned in relation to the parent pitches. The
  particular terrain equations, selection of modes and gain control
  were developed for this project; they are not copies of a
  named instrument or paper.

## FM-1 software examined for comparison and optimization

- **Frasil** by [Styves Goupil](https://github.com/styvesgoupil/frasil)
  (MPL-2.0): informed profiling, short-buffer noise staging and
  settled-filter-mode specialization. Volcano independently
  implemented the two retained techniques; no Frasil code was copied.
  Oscillator run-ahead and cached reciprocal BLEP experiments were
  evaluated and discarded. See [the headroom notes](ghost14-frasil-headroom.md).
- **Jiant**: a firmware used in practice and an instrument-design
  comparison, not a source of incorporated Volcano code.
- **AMB-1**: a reference for ambient/drone scope, not a source of
  incorporated code.
- **Arctic Scream**, **felucca-bsp** and **Melodee**: unlike the
  preceding comparison-only firmware, these are part of the actual
  CPU1 engineering lineage described above.
- [Groove OS](https://www.groove-os.com/), Peter Gombos: precedent
  for the idea of offering FM-1 firmware in a browser, not a
  source of emulator code in this project.

## Musical, instrumental and performance influences

These references concern sound, interaction and compositional approach;
they are **not attributions of code or hardware design reuse**.

- **Nicolas Godin**, especially “Acid Lake” in the score to
  *Fire of Love* (2022): the immediate musical impetus for Volcano's
  freely spaced, slowly unfolding pitch sequencing.
- **La Monte Young**, *Drift Studies*, and
  the Theatre of Eternal Music's *Day of Niagara*: antecedents for the
  source project's name and sustained-tone aesthetic.
- **Stylophone [CPM DS-2](https://stylophone.com/product/stylophone-ds-2/)**:
  continuously running dual-drone play and performance-centered control.
- **Moog [Subharmonicon](https://moogmusic-help.freshdesk.com/en/support/solutions/articles/69000867055-moog-subharmonicon-an-introduction-to-polyrhythm-subharmonics)**:
  subharmonic pitch relationships and independent rhythmic structures.
- **SOMA [Lyra-8](https://somasynths.com/lyra-organismic-synthesizer/)**:
  touch-based, continuously evolving organismic drone performance.
- **Make Noise [Strega](https://www.makenoisemusic.com/synthesizers-and-controllers/strega/)**:
  momentary intervention and touch-routing as performance ideas.
- **SOMA [Cosmos](https://somasynths.com/cosmos/)**:
  the idea of drifting musical memory and sound continuing beyond
  a player's immediate gesture. This is not a claim that Volcano
  implements Cosmos-style looping or capture.
- **Maryanne Amacher**, *Sound Characters (Making the Third Ear)*:
  a conceptual reference for emergent auditory identities and
  combination-tone listening. The DSP-generated GHOST intermodulation
  is **not** a physical reproduction of Amacher's listener-dependent
  otoacoustic phenomena.
- Other instruments and methods considered during design conversations
  include **ELTA Solar 42**, **Torso S-4**, **Hologram Microcosm**,
  asynchronous tape-delay practices associated with **Brian Eno**,
  and sound-texture approaches in film music. These remain broad
  references, not specific implementation or endorsement claims.

## Current features versus possible future directions

The tested C-headroom build contains **GHOST mode 14**:
Wave Terrain feeding an eight-mode resonant body, on CPU1 alongside
the shared FX pipeline. The previously explored Cross-Synth modes and
continuous mode-5-to-mode-14 morph are **not** features of this
stable candidate; they exist on separate experimental/development
branches. The present sixteen-key performance layout retains
eight subtractive per-drone keys and eight pre-existing additive/
macro gestures.

A proposed four-key set of **momentary resonant excitations**
(strikes, pulses, plucks, noise attacks, or other interventions)
has been discussed, inspired partly by Strega's immediacy and
the broader language of physical excitation. **No four-key
excitation set has been specified or implemented.** This section
records design context only; the working firmware and current
gesture assignments are unchanged.

## Scope of authorship

Volcano-specific changes and much of the documentation have been
developed with ChatGPT under the maintainer's direction and testing.
That does not diminish or replace the credits or licenses of Niagara
Drift and its earlier contributors. See [About Volcano](manual/about.md)
and [LICENSING.md](../LICENSING.md).
