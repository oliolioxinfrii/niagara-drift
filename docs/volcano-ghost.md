<!-- SPDX-License-Identifier: GPL-3.0-only -->
# Volcano GHOST — relational synthesis listening experiments

**Status: browser-only experiment, draft PR #8.** Branch:
`feature/volcano-ghost-intermodulation`. Do not merge or flash the
M-VAVE FM-1 before hardware timing, antialiasing, and full system checks.
The two parent drones, their sequencing and modulation, saved-project
schema, common FX, and other panel assignments are unchanged.

## Panel controls (Batch 2)

- **ALGORITHM** now controls **MOVEMENT** (0–100, existing overall LFO
  movement-depth scaling). This is a *control swap*, not a change to the
  meaning or time constants of MOVEMENT.
- **SEL + ALGORITHM** now controls saved **GHOST level** (0–100).
  A level of zero mutes new GHOST excitation and output; scene persistence
  and old projects retain their parameter values.
- **PRESETS** remains MORPH TIME.
- **SEL + PRESETS** cycles five temporary comparison types:
  `ORIGINAL`, `RESONANT BODY`, `WAVE TERRAIN`, `HETERODYNE`,
  `CROSS-SYNTH`. The type is RAM-only, resets to Original on restart,
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

## Comparison algorithms

| Mode | Principle | Listening objective |
| --- | --- | --- |
| Original | Existing cubic/quadratic intermodulation (unchanged) | Reference: complex warbling and broad nonlinear products |
| Resonant Body | Eight damped modes driven by quadratic/cubic cross-products, with pitch-related and one inharmonic resonance | Sympathetic vibrations and a coherent sustained presence |
| Wave Terrain | Non-separable curved 2-D wave surface; mixed finite difference removes either source's isolated contribution | Relational nonlinear colors, restrained harmonic transformation |
| **Heterodyne** | **NEW:** three pitch-tracked complex partial filters on each parent, then selected products `x_i * conj(y_j)` | A small set of identifiable, often low difference-frequency tones without the equivalent upper sum products |
| **Cross-Synth** | **NEW:** two twelve-band spectral filterbanks; each parent's band excitation is recolored by the other parent's measured spectral envelopes, then both directions are combined | Spectral transplantation and evolving harmonic structure, not simply polynomial distortion |

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
combined RMS at full GHOST level) before the shared GHOST compressor.
The two drones and the sequencer are untouched.

## Validation and status

`tests/host/drone_test.c` now checks all five modes for an output
difference relative to a GHOST-off reference, distinct results, and
a requirement for two active parents. It also checks Resonant Body
levels 0, 50 and 100, and both isolated output-monitor shortcuts.
`tests/scenarios/volcano-ghost.omni` checks five-mode selector
wraparound, the swapped ALGORITHM assignments, saved GHOST level, and
independent monitor toggles.

The algorithms are intentionally **not optimized**: all five DSP paths
are computed while GHOST is active, so crossfades can transition
immediately and every mode's calibrator has a stable history.
The earlier `ghost_fft.c` remains an unreferenced experiment and is
not compiled.

The source, test assertions and docs are committed. Full C host
tests, scenarios, and the Emscripten web build must still be run on
the user's Ubuntu machine; there is no verified full CI or rendered
audio result for this revision. A passing compile does not establish
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
