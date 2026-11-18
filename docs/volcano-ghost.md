<!-- SPDX-License-Identifier: GPL-3.0-only -->
# Volcano GHOST — fifteen-mode relational synthesis experiment

**Status: browser-only listening experiment; draft PR #8.** Branch:
\`feature/volcano-ghost-intermodulation\`. Do not merge into production
or flash an M-VAVE FM-1 until the host tests, device CPU/RAM budget,
and actual hardware build have been verified.

This experiment leaves both parent drones, their oscillators and filters,
sequencer, modulation, project schema and shared FX untouched. Every
GHOST sound depends on **both generated drone signals**. It is not an
independent oscillator or a third sequencer track.

## Controls

- **ALGORITHM** = MOVEMENT (existing LFO movement-depth scaling).
- **SEL + ALGORITHM** = saved **GHOST MIX**, 0–100:
  - 0: original direct Drone I + Drone II sound, no audible GHOST
  - 50: equal-power crossfade midpoint
  - 100: no direct parent output, entirely GHOST before shared FX
- **PRESETS** = MORPH TIME, as before.
- **SEL + PRESETS** = the **15 comparison modes below**. One encoder event
  advances one mode, even if the browser drag reports multiple detents;
  negative movement reverses. An ~80 ms fade reduces clicks.
- **SEL + white Key 1 (F3)** = FULL MIX / GHOST ONLY monitor. Removes
  direct parent output *after the drones have generated GHOST*.
- **SEL + white Key 2 (G3)** = GHOST ON / MUTED, leaving GHOST MIX fixed.
  Both diagnostics use ~12 ms fades, are not saved, and reset on restart.

The GHOST algorithm selection is transient and **now starts at
Resonant Body (mode 0)**. No project format changes. Saved GHOST MIX
parameter values are retained, but they have their new crossfade
interpretation, not the earlier additive-level interpretation.

### Dry/wet law and common output gain

Let \`t = P_GHOST/100\`. The direct parent contribution has multiplier
\`sqrt(1-t)\`; the selected GHOST voice has coefficient \`sqrt(t)\`
**before** a common post-compressor GHOST makeup multiplier of
\`1.70×\` (+4.61 dB). Both crossfade coefficients have ~20 ms slew.

| MIX | Parents (linear multiplier) | GHOST (incl. 1.70× makeup) |
| ---: | ---: | ---: |
| 0 | 1.000 | 0 |
| 25 | 0.866 | 0.850 |
| 50 | 0.707 | 1.202 |
| 75 | 0.500 | 1.472 |
| 100 | 0 | 1.700 |

These are amplitude coefficients, **not percentages of perceived
loudness**. The seven former comparisons varied greatly in timbre
and level. The existing moderate GHOST compressor still runs before
the final 1.70× gain, followed by the common instrument effects and
master soft limiter.

### Active engines and selectable mixtures

All five engines are evaluated once per GHOST-enabled audio sample
(in parallel). Eight ordinary blends are computed from their
*already calibrated* output signals, not by re-running their DSP.
There are two additional structurally different modes.

Notation: **B** Resonant Body, **T** Wave Terrain, **H** Heterodyne,
**C** Cross-Synth, **F** Coupled Feedback.

| Index | On-screen name | Structure |
| ---: | --- | --- |
| 0 | RESONANT BODY | B alone |
| 1 | WAVE TERRAIN | T alone |
| 2 | HETERODYNE | H alone |
| 3 | CROSS-SYNTH | C alone |
| 4 | COUPLED FB | F alone |
| 5 | CROSS + BODY | 70% C + 30% B |
| 6 | CROSS + TERRAIN | 70% C + 30% T |
| 7 | CROSS + FB | 70% C + 30% F |
| 8 | TERRAIN + BODY | 60% T + 40% B |
| 9 | BODY + FB | 65% B + 35% F |
| 10 | CROSS + HETERO | 70% C + 30% H |
| 11 | CROSS/BODY/FB | 55% C + 30% B + 15% F |
| 12 | CROSS/TERR/BODY | 50% C + 30% T + 20% B |
| 13 | BODY LO/TERR HI | Low-pass B + high-pass T, crossover ~350 Hz |
| 14 | TERRAIN -> BODY | Wave Terrain excites a separate eight-mode resonator bank |

The percentages are **linear mixture coefficients**. They do not
guarantee equal RMS or perceived loudness, since all outputs are
related and can be correlated or cancel in some registers.

For each mode **5–14**, the final combined signal has a separate
slow RMS detector and gain correction, updated over ~650 ms with a
gain bound of 0.48×–3×, followed by gentle bounded waveshaping.
The goal is reasonably consistent comparison without erasing each
component's dynamics. Individual algorithms 0–4 retain all their
previous calibration and DSP settings.

### The five retained synthesis mechanisms

**Resonant Body:** Eight driven, damped complex resonators tuned to
combinations of the two performed drone pitches. The cavity's
decay constants span ~0.16–0.42 s. The standalone Body output
retains the boosted ~68% parent-RMS calibration target.

**Wave Terrain:** A two-dimensional non-separable nonlinear surface.
Only the mixed finite difference of the surface is output:
\`F(x,y)-F(x,0)-F(0,y)+F(0,0)\`. Consequently either source
alone generates no sustained Terrain output.

**Heterodyne:** Three complex, pitch-tracked partial filters per drone.
Selected complex products isolate a limited set of difference-frequency
components, with low-frequency DC rejection.

**Cross-Synth:** Two 12-band state-variable-filter banks whose
per-band audio is colored by the *other* drone's spectral-envelope
measurements. The two directions are combined, without new oscillators.

**Coupled Feedback:** Two damped, nonlinear delay networks driven
by their separate parent drones; the opposite network modulates each
feedback coefficient. Only cross-dependent products become audible.
The feedback design remains contractive in the absence of input.

### Two structurally different experimental modes

**13 — LOW BODY/HIGH TERR:** Each calibrated engine passes through
an independent two-stage one-pole low-pass, at approximately 350 Hz.
BODY contributes its two-pole low-pass result; TERRAIN contributes
the complementary two-pole high-pass \`x-2Lx+L²x\`.
This deliberately assigns the slower resonant sound and nonlinear
terrain texture to different frequency regions. The crossover is
fixed in this prototype.

**14 — TERRAIN -> BODY:** Raw Wave Terrain output, not the ordinary
BODY output or BODY excitation, passes through a bounded excitation
shaper, driving a **second bank** of eight damped modal states.
This bank shares the original Body's pitch-related tuning
coefficients but has completely independent memory. Consequently
this mode is a serial instrument architecture, *not* a linear sum
of two completed GHOST outputs.

### Retired code

- **Original:** the original cubic/quadratic intermodulation,
  slow original-only output gain and waveshaper have been removed.
  The pre-existing private two-pole input filtering and parent-power
  detection were **preserved** because all five retained engines
  rely on those source samples or energy estimates.
- **Live Convolution:** the 1,024-point FFT, 512-tap moving-IR engine,
  all convolution buffers and per-mode gain tracking are **no longer
  compiled**. \`firmware/src/dsp/ghost_batch3.c\` now contains only
  Coupled Feedback. The historical \`ghost_fft.c\` experiment also
  remains unreferenced and is not compiled.

The new mix/blend logic is in \`firmware/src/dsp/ghost_blends.c\`,
included by \`drone.c\` to work with the existing single-translation-unit
Emscripten build. Parent voices and sequencer are not modified.

## Testing and experimental limitations

- \`tests/scenarios/volcano-ghost.omni\` now cycles all **15** mode
  positions and verifies forward/reverse wraparound. It also checks
  the ALGORITHM/MOVEMENT swap, saved GHOST MIX and both monitor toggles.
- \`tests/host/drone_test.c\` checks the standalone and blended
  GHOST signals, parent dependency, output-mute diagnostics, and
  pure-dry / pure-wet endpoints at fixed GHOST MIX settings.
- \`tests/host/ghost_blends_test.c\` checks the exact mixture weights,
  spectral-split difference, excitation-transfer resonance and decay,
  and independent blend gain bounds.
- \`tests/host/ghost_batch3_test.c\` now exercises only retained
  Coupled Feedback, since the convolution FFT has been removed.
- \`tests/run_tests.sh\` includes both dedicated checks.

The source was updated through GitHub. **Full host regression,
Emcc WebAssembly compilation, performance budgets and listening have
not yet been verified for this new revision.** The host
\`drone-cost\` test may fail because five engines and additional
modal/spectral blend operations run continuously in the browser
prototype; do not waive that test for eventual physical-device use.

For accurate A/B comparisons, set global FEEDBACK to zero and let
shared delay/reverb tails settle: both monitor modes remove direct
contributions before the shared effects, not old tails.

## Home Ubuntu / WSL runbook

\`\`\`bash
cd ~/niagara-drift
source .venv/bin/activate
source ~/emsdk/emsdk_env.sh
git status -sb
git fetch origin
git switch feature/volcano-ghost-intermodulation
git pull --ff-only

# Recommended once for the new 15-mode changes:
sh tests/run_tests.sh

# Browser rebuild, needed after updating the branch:
rm -f build/emu/omni.wasm
sh web/emu/build.sh
python3 -m http.server 8766 --directory build/emu
\`\`\`

Open \`http://localhost:8766/\` and hard-refresh. The test suite
is optional for subsequent listening sessions if no code has changed.
The firmware should remain on the experimental branch; do not flash.
