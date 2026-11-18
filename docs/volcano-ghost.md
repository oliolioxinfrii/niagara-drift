<!-- SPDX-License-Identifier: GPL-3.0-only -->
# Volcano GHOST — relational synthesis listening experiments

**Status: browser-only experiment, draft PR #8.** Branch:
`feature/volcano-ghost-intermodulation`. Do not merge or flash the
M-VAVE FM-1 before hardware timing, antialiasing, and full system checks.
The two parent drones, their sequencing and modulation, saved-project
schema, common FX, and other panel assignments are unchanged.

## Panel controls (Batches 1–3)

- **ALGORITHM** now controls **MOVEMENT** (0–100, existing overall LFO
  movement-depth scaling). This is a *control swap*, not a change to the
  meaning or time constants of MOVEMENT.
- **SEL + ALGORITHM** now controls the saved **GHOST MIX** (0–100),
  instead of an additive GHOST level. At **0** the output is the unchanged
  Drone I + Drone II pair; at **50** the dry and GHOST paths use equal-power
  crossfade coefficients (each approximately 0.707 before wet makeup); at
  **100** the direct parents vanish and only GHOST enters the shared FX.
  **The source drones ALWAYS continue generating GHOST.**
  Parameter values still persist in scenes and old projects, but their
  meaning changes on this experimental branch. In particular a saved
  GHOST value of 100 now selects GHOST-only rather than full parents
  plus maximum GHOST. Newly migrated projects still start at 0.
- **PRESETS** remains MORPH TIME.
- **SEL + PRESETS** cycles seven temporary comparison types:
  `ORIGINAL`, `RESONANT BODY`, `WAVE TERRAIN`, `HETERODYNE`,
  `CROSS-SYNTH`, `LIVE CONV`, `COUPLED FB`.
  The type is RAM-only, resets to Original on restart,
  and crossfades over approximately 80 ms.
- **SEL + white Key 1 (F3)** toggles **GHOST ONLY / FULL MIX** by
  removing just the parent drones' contribution to the pre-effects mix.
  Parent synthesis and GHOST excitation continue running.
- **SEL + white Key 2 (G3)** independently toggles **GHOST MUTED /
  GHOST ON**, leaving its algorithm and saved level alone. The two
  diagnostics each crossfade over about 12 ms, reset at restart, and do
  not trigger their ordinary STRIP/DARK gestures or sequencer note entry.

**Effects caveat:** Neither monitor makes a separate FX bus. Existing
delay and reverb tails continue temporarily and global FEEDBACK can
receive the changed monitored mix. Disable FEEDBACK and allow FX tails
to decay for reliable A/B tests.

### Experimental dry/wet mixing and makeup gain

GHOST MIX uses a **20 ms ramped equal-power crossfade**. With
`t = GHOST_MIX / 100`, the direct parent-pair multiplier is
`sqrt(1-t)`, and the relational output's multiplier is `sqrt(t)`.
For a more usable GHOST level, **every algorithm receives 1.70×
(+4.61 dB) makeup gain after the existing compressor**, but before
the shared mix and master output effects. The equal-power rule refers
to coefficients, not equal perceived loudness; the seven algorithms
have markedly different spectra and envelope behaviors.

| GHOST MIX | Parent direct level | GHOST coefficient including makeup |
| --- | ---: | ---: |
| 0 | 1.000 | 0.000 |
| 25 | 0.866 | 0.850 |
| 50 | 0.707 | 1.202 |
| 75 | 0.500 | 1.472 |
| 100 | 0.000 | 1.700 |

These are linear audio multipliers, *not* percentage shares of the
resulting perceived loudness. The downstream master clipping/limiting
can further modify the actual peak and loudness balance.

SEL+F3 still removes just the direct parents, independently of the
crossfade setting; SEL+G3 still gates GHOST audio without modifying
the saved position. With GHOST MIX at 100 and GHOST muted, the output
becomes silent after shared effects tails have cleared. GHOST MIX at
0 is dry-only; toggling SEL+G3 then has no audible consequence.

**This is a mixing experiment, not a redesign of Live Convolution.**
The seven algorithms are unchanged. The aim is to separate the issue
of timbral quality from the strong spectral masking that can occur
when GHOST is merely added beneath two full-volume drones.

## Comparison algorithms

| Mode | Principle | Listening objective |
| --- | --- | --- |
| Original | Existing cubic/quadratic intermodulation (unchanged) | Reference: complex warbling and broad nonlinear products |
| Resonant Body | Eight damped modes driven by quadratic/cubic cross-products, with pitch-related and one inharmonic resonance | Sympathetic vibrations and a coherent sustained presence |
| Wave Terrain | Non-separable curved 2-D wave surface; mixed finite difference removes either source's isolated contribution | Relational nonlinear colors, restrained harmonic transformation |
| **Heterodyne** | Three pitch-tracked complex partial filters on each parent, then selected products `x_i * conj(y_j)` | A small set of identifiable, often low difference-frequency tones without the equivalent upper sum products |
| **Cross-Synth** | Two twelve-band spectral filterbanks; each parent's band excitation is recolored by the other parent's measured spectral envelopes, then both directions are combined | Spectral transplantation and evolving harmonic structure, not simply polynomial distortion |
| **Live Convolution** | **Batch 3:** Each parent convolves with a short evolving impulse response captured from the other drone; true 512-tap bidirectional FIR convolution implemented using 1024-point FFT overlap-save | A spectral and temporal imprint: moving resonances, texture and a sense of an evolving acoustic medium |
| **Coupled Feedback** | **Batch 3:** Two parent-excited delay networks cross-modulate their bounded feedback; only the cross-response is heard | Interacting memory, shifting beats, nonlinear and possibly unstable-sounding movement without actual self-oscillation |

The new Batch 2 implementation is `firmware/src/dsp/ghost_batch2.c`,
included by the existing `drone.c` compilation unit. No extra
independently pitched GHOST oscillator is present. Both modes are
generated from the two performed, post-filter parent signals.

### Resonant Body loudness change

Resonant Body's bounded adaptive calibration now targets approximately
**68%** of the parents' combined RMS at GHOST 100, instead of
**40%**. Its maximum gain bound rises from 32× to 48×.
Wave Terrain remains at 40%; the existing GHOST compressor
is retained. This is a *nominal electrical level target*,
not a guaranteed perceived loudness increase: compression and
soft saturation can reduce the realized difference. All modes still
multiply by the saved 0–100 GHOST amount, so **GHOST = 0 is mute**.

### Batch 2: selective heterodyning

The complex narrow-band filters track harmonics 1, 2, and 3 of each
performed drone fundamental. Their bandwidths are approximately 4.5%
of center frequency (minimum 7 Hz), and they are driven by their
respective source audio, not by an independent third voice.
Five chosen cross-products are reconstructed using the real part of
`X_i * conjugate(Y_j)`. This selects **difference** components,
unlike direct real-signal multiplication, which produces sums as well.
A slow DC rejection removes near-zero differences. If the drones are
in exact unison and both contain only their fundamentals, the result
can be weak by construction; harmonics and detuning create additional
relationships. The goal is musical selectivity, not always-on noise.

### Batch 2: reciprocal cross-synthesis

The two post-filter drones enter 12 topology-preserving state-variable
bandpass filters per input, centered between ~35 Hz and ~2.4 kHz.
Each band tracks the magnitude envelope of its own source.
The bandpass output from drone I is weighted by the **spectral envelope
of drone II**, and vice versa. A modest broad-band energy-dependent
pedestal allows interaction when the two sources have disjoint partials;
the contribution is still gated by the *other* source's measured
presence. This is a **filterbank spectral-envelope cross-synthesis**,
not an FFT-bin multiplication, granular pitch shift, or convolution.
It is designed to avoid the near-inaudibility of the earlier sparse
FFT experiment.

Both new modes have independent RMS detectors and bounded
source-relative gain calibration (nominal 56% and 48% of the parents'
combined RMS) before the shared GHOST compressor.
The two drones and the sequencer are untouched.

### Batch 3: Live Convolution

`firmware/src/dsp/ghost_batch3.c` adds an independent moving-IR convolution
engine. At every 512 input samples (~11.6 ms), the most recent block of
each performed drone becomes a 512-tap, decaying impulse response for the
*other* drone. The responses are smoothed between blocks (70% retained,
30% newly captured). The captured response retains the donor's amplitude;
it is **not normalized into an always-on filter**.

The two actual, full convolutions are computed by complex spectral
multiplication and 1024-point radix-2 inverse FFT using overlap-save.
These are full products of the source and response transforms, not the
partial/bin-selected intermodulation of the earlier Spectral experiment.
The two convolution directions are averaged. This introduces approximately
one 512-sample block of processing latency and finite IR memory.

A signal from only one drone may leave a short residual as an existing
response decays, but it cannot continually generate new GHOST output
without the other source. Convolution can emphasize shared spectral
content: if the drones have very different fundamentals or spectral
structures, the response may be weak. The moving, shortened IR keeps
broader contributions than a long, isolated sinusoidal IR.

### Batch 3: Coupled Feedback

Two driven delay loops have principal delays of 1207 and 1889 samples
(~27.4 ms and ~42.8 ms), plus secondary taps. Their per-sample
feedback coefficients are modulated by the opposite loop's signal;
the cross-signal is **not** used as a separate drive. Every audible
output term contains a factor from *both* loops.

The feedback network is contractive in the absence of sources:
the maximum feedback coefficient is 0.85, and the secondary tap
contributes at most 0.08 (sum below 1). Saturation is bounded by
`tanh`. Thus it can ring and behave irregularly while excited, but
it cannot sustain a self-running oscillator after the sources stop.
A low-pass and DC remover condition the resulting cross-dependent signal.

The Batch 3 modes have separate source-relative gain calibration
(nominal targets 58% for Live Convolution and 52% for Coupled Feedback)
and feed the same shared GHOST compressor. These are
relative electrical targets, not validated loudness equivalence.

## Validation and status

`tests/host/drone_test.c` now checks all seven modes as isolated
GHOST outputs against a dry reference, distinct results, and the
requirement for two active parents. Fixed-mix GHOST-mute A/B tests
separate real GHOST energy from the change in parent volume. It also checks Resonant Body
levels 0, 50 and 100, and both isolated output-monitor shortcuts.
A separate `tests/host/ghost_batch3_test.c` verifies the convolution FFT's
roundtrip accuracy, measurable output from two different pitched inputs,
two-source dependency, and finite decay after excitation ends for both
new algorithms. This is invoked by `tests/run_tests.sh` as `ghost-batch3`.
`tests/scenarios/volcano-ghost.omni` checks seven-mode selector
wraparound, the swapped ALGORITHM assignments, saved GHOST level, and
independent monitor toggles.

The algorithms are intentionally **not optimized**: all seven DSP paths
are computed while GHOST is active, so crossfades can transition
immediately and every mode's calibrator has a stable history.
The earlier `ghost_fft.c` remains an unreferenced experiment and is
not compiled.

The source, revised mix tests and docs are committed. Full C host
tests, scenarios, and the Emscripten web build must still be run on
the user's Ubuntu machine; there is no verified full CI or rendered
audio result for this revision. The full test suite contains a separate
`drone-cost` hardware timing budget; that may fail by design while
seven browser-only algorithms are calculated concurrently. Do not treat
a deliberate hardware-budget failure as proof that the browser synthesis
does not function, but do not waive it for eventual FM-1 deployment. A passing compile does not establish
musical efficacy, and browser CPU does not establish FM-1 headroom.

### Ubuntu/WSL runbook

```bash
cd ~/niagara-drift
source .venv/bin/activate
source ~/emsdk/emsdk_env.sh
git status -sb
git fetch origin
git switch feature/volcano-ghost-intermodulation
git pull --ff-only
sh tests/run_tests.sh
rm -f build/emu/omni.wasm
sh web/emu/build.sh
python3 -m http.server 8766 --directory build/emu
```

The browser is at `http://localhost:8766/`. Hard refresh after
rebuilding. Do **not** build or flash hardware as part of this experiment.
