<!-- SPDX-License-Identifier: GPL-3.0-only -->
# GHOST: relational drone experiment

Status: **experimental**. Created from `main` on `feature/volcano-ghost-intermodulation`. Not yet verified on FM-1 hardware; do not merge or flash until host, anti-aliasing and CPU tests pass.

## Playing

- **ALGORITHM** sets GHOST level (0–100, default 0).
- **SEL + ALGORITHM** retains the previous **MOVEMENT** setting.
- No independent pitch, waveform, sequence, gesture, or GHOST tone visualization.
- GHOST belongs to the sound and user-scene presets. Format 45 projects and A–F scene banks migrate to format 46 with GHOST initialized to zero.
- GHOST enters the existing stereo mix *before* the shared delay, drive, and reverb; it has no separate effects path. TAIL gates new interaction excitation and allows existing common FX tails to ring.

## DSP experiment

At each audio sample, let `x` and `y` be the two post-filter drone outputs **before** the per-drone TAIL output attenuation. After duplicating and low-pass filtering these signals:

```text
Px[n] = (1-alpha)*Px[n-1] + alpha*u[n]^2
Py[n] = (1-alpha)*Py[n-1] + alpha*v[n]^2
Pxy[n] = (1-alpha)*Pxy[n-1] + alpha*u[n]*v[n]

G3 = (u*u - Px)*v + (v*v - Py)*u
G2 = u*v - Pxy
E  = 9.0*G3 + 0.50*G2
```

A fixed soft limiting curve bounds `E`. A high-pass and low-pass remove subsonic DC and constrain the final spectrum. Only the result is mixed, after multiplying by GHOST level. This is not a pitched third oscillator and has no independently tuned resonator.

Current filtering is two cascaded one-poles on **each private input copy**, then a high-pass and a low-pass on GHOST. Filtering reduces but does **not eliminate** aliases from nonlinear multiplication. The two source filters and the output filter are provisional until high-frequency alias tests and listening renders exist. The present level coefficients are likewise exploratory.

The original drone samples are untouched by the new processing. When GHOST is zero, the nonlinear stage bypasses all per-sample work.

## Validation checklist

- Run `sh tests/run_tests.sh` and the host/web emulator.
- Run `sh host/build_host.sh`; render both GHOST-off and GHOST-on versions of the factory P1 NIAGARA patch for at least 60 seconds.
- Compare source and ghost-only spectrograms, sustained beating, and output RMS; test both headphones and the internal speaker.
- Render high-pitched saw/square source combinations and inspect Nyquist-folded products.
- Run `tests/host/drone_cost.c` case 10 and `tools/budget.py`; on hardware measure worst-block load with `tools/fm1_cpu.py`.
- Verify TAIL, PLAY stop, shared FX decay, two-voice dependence, and project-format migration before merging.
