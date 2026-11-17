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
| **Original** | Unchanged cubic/quadratic intermodulation generator, now with a gentle common compressor | Retain the cinematic low warble while evening its level |
| **Sub -12** | The same GHOST interaction shifted **down one octave**; no unshifted Original | Deep relational movement, and whether granular pitch-down works alone |
| **Octave +12** | The same GHOST interaction shifted **up one octave**; no unshifted Original | The intense electrical upper register, without Original masking the comparison |
| **Difference** | A distinct, **quadratic-only** `x*y` interaction with two ~245 Hz low-pass poles and DC removal | Test whether sparse low difference-frequency components work better than broad cubic spectral filling |

The new **Difference** mode remains nonlinear, but does *not* reuse the broad cubic GHOST spectrum. It selects the lowest products of direct cross-multiplication and tests a simpler generative architecture. There is still no third oscillator, independent pitch or separate sequencer. A future mode could investigate delay-coupled or other non-polynomial relationships, but this test first isolates whether limiting spectral density is valuable.

**Dynamic range test, all four modes:** Original's existing slow parent-relative gain calibration is retained. After the selected mode and 80 ms mode-change crossfade, a moderate GHOST-only compressor runs before the GHOST knob scales the result and before the shared FX. Its power detector has approximately **16 ms attack and 375 ms release**; the knee spans approximately **6 dB** around **-18.4 dBFS**, with **2:1 compression above** the threshold and restrained **upward levelling below** (max +3.5 dB). The intent is to retain natural dynamics while making quiet interactions somewhat more present and loud interactions less competitive. This is **not a hard output limiter or perceived-loudness normalization**. The GHOST level at 100 can still be intense, and the shared effects can add energy.

The Sub and Octave shifts use a shared 4096-sample delay ring and complementary overlap-add windows of 2048 samples. The octave-up head advances 2 input samples per output; the octave-down head advances 0.5 with interpolation. Both preserve real-time output duration, but the ~46 ms grain cycle can produce strong ~21.5 Hz-spaced sidebands. Previous experiments found this coloration sometimes more prominent than the nominal octave frequencies. Consequently these are **colored, nominal-octave granular shifts**, not clean harmonizers. In this round, they are intentionally heard *separately*, rather than blended together or with Original.

The parent sources and their sequenced/modulated behavior are untouched. With either parent silent, no new two-source interaction is excited; any residual low-pass or grain content decays. All modes pass through the existing shared delay, drive and reverb. No additional display, parameter, project format, or saved mode has been added.

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

A polynomial soft limiter bounds `E` without a per-sample division. A high-pass and low-pass remove subsonic DC and constrain the final spectrum. The GHOST prototype measures slow output power and the two parents' power and retains a bounded, 2-second-slewed gain calibration (nominal target roughly 45% of the parents' combined RMS at level 100, subject to limiting and a 32x maximum gain). The four listening variants then pass through a common moderate, upward/downward dynamic compressor before the saved GHOST level is applied. Neither stage is a spectral peak picker, a third oscillator, or a hard limiter. These are experimental coefficients, not yet loudness-matched by listening or measured against the existing scene library.

Current filtering is two cascaded one-poles on **each private input copy**, then a high-pass and a low-pass on GHOST. Filtering reduces but does **not eliminate** aliases from nonlinear multiplication. The two source filters and the output filter are provisional until high-frequency alias tests and listening renders exist. The coefficients and loudness calibration are exploratory and require render-based verification; the first uncalibrated version was reported inaudible on P1 NIAGARA with external headphones and an audio interface.

The original drone samples are untouched by the new processing. At GHOST zero, the nonlinear test modes do not contribute to the output. Mode selection does not alter the saved-project layout (format 46).

## Validation checklist

For this stage, prioritize the browser emulator and relative listening comparisons; do not treat FM-1 CPU measurements as gating the experiment.

- Run `sh tests/run_tests.sh` and the host/web emulator.
- Run `sh host/build_host.sh`; render both GHOST-off and GHOST-on versions of the factory P1 NIAGARA patch for at least 60 seconds.
- Compare source and ghost-only spectrograms, sustained beating, and output RMS at GHOST levels 25/50/100 and quiet/loud source levels using external headphones and monitors; verify the speaker-mute setting separately if needed.
- Render high-pitched saw/square source combinations and inspect Nyquist-folded products.
- Run `tests/host/drone_cost.c` case 10 and `tools/budget.py`; on hardware measure worst-block load with `tools/fm1_cpu.py`.
- Verify TAIL, PLAY stop, shared FX decay, two-voice dependence, and project-format migration before merging.
