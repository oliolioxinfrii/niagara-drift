<!-- SPDX-License-Identifier: GPL-3.0-only -->
# Volcano GHOST — relational synthesis experiments

Status: **browser-emulator experiment only**. Draft PR #8 on
`feature/volcano-ghost-intermodulation`. Do not merge or flash to FM-1
hardware before full testing and budget assessment. The parent drone engine,
sequencer, scenes and existing UI behavior remain unchanged.

## Playing

- **ALGORITHM** sets GHOST level (0–100). It is saved in sounds and scenes,
  defaults to 0 for migrated projects, and enters the existing stereo
  delay/drive/reverb bus before the shared effects.
- **SEL + ALGORITHM** remains MOVEMENT. **PRESETS** remains MORPH TIME.
- **SEL + PRESETS** cycles three comparison modes: `ORIGINAL`,
  `RESONANT BODY`, `WAVE TERRAIN`, in either direction. A ~80 ms crossfade
  reduces mode-switch clicks. The comparison mode is RAM-only and resets to
  Original on restart; it does not change the stored project format.
- The GHOST source is derived from the two *audible post-filter drones*
  including their Swell and TAIL attenuation. No independent drone oscillator,
  note track, sequencer, pitch knob or output effect is introduced.

## Batch 1: comparison modes

| Mode | Sound-generation principle | What to evaluate |
| --- | --- | --- |
| **Original** | Existing cross-only cubic and quadratic intermodulation, 2-pole private low-pass inputs, and slow adaptive gain calibration | Reference low warble, full broad-spectrum interaction |
| **Resonant Body** | Eight damped complex resonances **driven by cross-products of the two parent signals**, tuned to pitch-related sum/difference frequencies and one inharmonic cavity mode | A response that accumulates energy and decays: is a coherent, spatially suggestive body audible without becoming another autonomous oscillator? |
| **Wave Terrain** | The filtered parent waveforms become X/Y coordinates on a curved, non-separable 2D surface. Its *mixed finite difference* isolates the genuinely cross-dependent portion | Geometrical nonlinear timbres, irregular contours and changes in sound due to the *relationship* between the two trajectories |

Original's algorithm and the preexisting moderate GHOST-only compressor
(~16 ms attack, ~375 ms release, soft-knee 2:1 above ~−18.4 dBFS, capped
+3.5 dB low-level boost) are retained. Each new mode has an **independent**
RMS detector and gain factor, slowly targeting ~40% of the two parents'
combined RMS at GHOST 100 (limits 0.18×–32×), then passes through the same
compressor and the same saved GHOST amount. This allows moderate-level
comparison without requiring that one weak algorithm be turned to 100 just
to become measurable. Actual perceived loudness is **not** guaranteed matched.

### Resonant Body

A parent-audio cross-excitation
`(u*v - DC(u*v)) + 0.55*(u*u*v+v*v*u)` excites eight damped complex
resonators. Frequencies depend on the two *performed* parent pitches:
`|f0-f1|`, `f0+f1`, `|2f0-f1|`, `|2f1-f0|`,
`2f0+f1`, `f0+2f1`, `|3f0-2f1|`, and
`0.727*(f0+f1)`. They are clamped to 25–2900 Hz.
E-fold decay times range approximately 0.16–0.42 s.
Drones excite the bank continually; when either parent ceases, new
excitation vanishes, but the modal state can ring out briefly. It therefore
has a *finite memory*, not independent note generation. Unlike the older
Focused mode, Resonant Body does **not** add one resonator on top of Original
intermodulation: the body itself is the entire new GHOST voice.

### Wave Terrain

Let `F(x,y)` be a curved two-dimensional surface with nonlinear ridges and
valleys, where `x=tanh(2.8*u)` and `y=tanh(2.8*v)`. Both parent signal
amplitudes traverse the surface at audio rate; the relative slow parent
power bends one ridge.

The emitted value is proportional to:

```text
G(x,y) = F(x,y) - F(x,0) - F(0,y) + F(0,0)
```

By construction, `G(x,0)=G(0,y)=0`: either parent alone produces no
cross-dependent output (aside from the transient filter decay).
This is deliberately different from using an ordinary waveshaper on
`u+v` or letting drone I modulate drone II.

A 1-pole low-pass and slow DC remover shape the output.
This mode has no independent generator or preset sequence.

## Listening protocol and validation

Use the *same* drone parameters and GHOST amount when comparing modes. Start
around GHOST 25–60, with relatively stable parent tones. Compare unison,
near-unison, a fifth, widely separated pitches, and slowly modulated
sequenced passages. Because the two new modes can sound significantly
different from Original, do not judge the algorithms only at 100.

For **test validation**, `tests/host/drone_test.c` now compares each mode
to an identically initialized GHOST-off control, not merely to Original. It
checks a minimum measured difference-RMS, distinct mode outputs, the
dependency on both parents, and resetting to Original. The browser scenario
`tests/scenarios/volcano-ghost.omni` checks mode cycling and existing
knob mappings. Numerical amplitude checks are not perceptual listening tests.

**Status:** source integration and test cases committed, but the latest
revision has **not yet been compiled or validated by the existing host test
suite or the Emscripten web build**. The experimental code deliberately
prioritizes sound exploration over FM-1 memory/CPU efficiency; hardware
anti-aliasing, denormal and cost tests are still required. The previous
`ghost_fft.c` experiment remains in the branch as an unreferenced
historical test module; it is **not compiled** in Batch 1.
