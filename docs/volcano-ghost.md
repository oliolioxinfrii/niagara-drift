<!-- SPDX-License-Identifier: GPL-3.0-only -->
# GHOST: relational drone experiment

Status: **experimental**. Created from `main` on `feature/volcano-ghost-intermodulation`. Not yet verified on FM-1 hardware; do not merge or flash until host, anti-aliasing and CPU tests pass.

## Playing

- **ALGORITHM** sets GHOST level (0–100, default 0).
- **SEL + ALGORITHM** retains the previous **MOVEMENT** setting.
- **PRESETS** still sets Morph Time. **SEL + PRESETS** cycles four experimental GHOST algorithms; the chosen mode appears briefly in the top bar.
- The test mode is RAM-only: it starts on **Original** after restarting and is not saved in scenes or presets. GHOST amount is still saved.
- No independent source oscillator, user pitch parameter, new page, or GHOST tone visualization.
- GHOST belongs to the sound and user-scene presets. Format 45 projects and A–F scene banks migrate to format 46 with GHOST initialized to zero.
- GHOST enters the existing stereo mix *before* the shared delay, drive, and reverb; it has no separate effects path. TAIL gates new interaction excitation and allows existing common FX tails to ring.

## Four GHOST listening modes (browser simulator)

Hold **SEL** and turn **PRESETS**. The four modes cycle in both directions, with an approximately 80 ms transition. Leave ALGORITHM at the same GHOST amount for a controlled comparison.

| Mode | Processing | What it tests |
| --- | --- | --- |
| **Original** | Unchanged cross-only cubic/quadratic interaction, including each drone's Swell | Preserves the cinematic low-register warble; reference |
| **Orig + Oct** | 95% Original + 55% granular octave-up copy | Preserve the original bass/wobble while adding the electrical upper register |
| **Sub + Oct** | 68% octave-down copy + 58% octave-up copy (no dry Original) | Whether octave displacement makes a new dual-register voice |
| **Focused** | 74% Original + a narrow, driven resonator centered on `2*f(I)+f(II)` | Test a distinct, pitch-following upper combination tone without adding an oscillator |

The two shifted copies use one 4096-sample delay ring and complementary grain windows of 2048 samples. The octave-up read head advances 2 input samples per output; the octave-down head advances 0.5 samples per output with linear interpolation. Both maintain real-time output duration, but the ~46 ms grain cycle can impart additional roughness or rhythmic artifacts. These are audible experiments rather than transparent pitch shifters.

**Focused** is not a third generator. It is a narrow, decaying resonant filter excited by GHOST's existing interaction signal, with its center frequency calculated from the two parent drone pitches. Its target (the cubic **sum** product) changes with the parents' pitches. A slow bounded boost aims to make that component audible without giving it independent excitation. If one parent stops, the resonator decays; GHOST cannot continue playing its own notes.

Compared with the prior listening set, **Upper** and **Sustained** have been removed because neither produced a reliably audible distinction from Original during headphone testing. User listening also confirmed GHOST becomes inaudible with one drone off, while the preceding Octave Mix generated a strong electrical/substation-like buzz but sacrificed much of the original low body. The revised **Orig + Oct** deliberately restores that body. The alternative mixes are bounded in amplitude before the shared effects, but **are not perceptually loudness-matched**. At level 100, shared summing and effects can still produce substantially louder, harsher material; compare at a moderate GHOST level before exploring 100.

All modes still pass through Volcano's existing stereo delay, drive, and reverb. No additional screen, oscillator, project format, or user pitch control was added.

**Simulator only for now:** The extra 4096-float octave buffer and dual processing paths are deliberate exploratory costs. Hardware RAM/CPU and anti-aliasing have not been verified. This branch should not yet be built or flashed as release firmware.

## DSP experiment

At each audio sample, let `x` and `y` be the two post-filter drone outputs, multiplied by their **audible Swell envelopes** and per-drone TAIL attenuation. The common PLAY fade applies afterward to the three-voice mix. After duplicating and low-pass filtering these signals:

```text
Px[n] = (1-alpha)*Px[n-1] + alpha*u[n]^2
Py[n] = (1-alpha)*Py[n-1] + alpha*v[n]^2
Pxy[n] = (1-alpha)*Pxy[n-1] + alpha*u[n]*v[n]

G3 = (u*u - Px)*v + (v*v - Py)*u
G2 = u*v - Pxy
E  = 9.0*G3 + 0.50*G2
```

A polynomial soft limiter bounds `E` without a per-sample division. A high-pass and low-pass remove subsonic DC and constrain the final spectrum. The GHOST prototype now measures its slow output power and the two parents' power, then applies a bounded, 2-second-slewed gain calibration (target roughly 45% of the parents' combined RMS at level 100, subject to limiting and a 32x maximum gain). This is intended to keep cubic products audible on quieter patches while retaining faster beats and swells; it is **not** a spectral peak picker or third oscillator. Only the result is mixed, after multiplying by GHOST level.

Current filtering is two cascaded one-poles on **each private input copy**, then a high-pass and a low-pass on GHOST. Filtering reduces but does **not eliminate** aliases from nonlinear multiplication. The two source filters and the output filter are provisional until high-frequency alias tests and listening renders exist. The coefficients and loudness calibration are exploratory and require render-based verification; the first uncalibrated version was reported inaudible on P1 NIAGARA with external headphones and an audio interface.

The original drone samples are untouched by the new processing. At GHOST zero, the nonlinear test modes do not contribute to the output. Mode selection does not alter the saved-project layout (format 46).

## Validation checklist

For this stage, prioritize the browser emulator and relative listening comparisons; do not treat FM-1 CPU measurements as gating the experiment.

- Run `sh tests/run_tests.sh` and the host/web emulator.
- Run `sh host/build_host.sh`; render both GHOST-off and GHOST-on versions of the factory P1 NIAGARA patch for at least 60 seconds.
- Compare source and ghost-only spectrograms, sustained beating, and output RMS; test both headphones and the internal speaker.
- Render high-pitched saw/square source combinations and inspect Nyquist-folded products.
- Run `tests/host/drone_cost.c` case 10 and `tools/budget.py`; on hardware measure worst-block load with `tools/fm1_cpu.py`.
- Verify TAIL, PLAY stop, shared FX decay, two-voice dependence, and project-format migration before merging.
