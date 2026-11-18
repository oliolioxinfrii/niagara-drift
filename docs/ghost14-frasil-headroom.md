<!-- SPDX-License-Identifier: GPL-3.0-only -->
# Architecture C: Frasil-inspired kernel headroom experiment

Branch: `experiment/ghost14-ghost-fx-cpu1-headroom`

Base: architecture C / STRESS V2, `f751a38`; the original tested
`experiment/ghost14-ghost-fx-cpu1` is intentionally unchanged.
`main` already contains the previous Frasil-inspired changes, but has
diverged from GHOST/dual-core. **Do not merge all 19 headroom commits
blindly**: this is a selective port of the two retained sample kernels.

Two independent, **default-on** switches in `firmware/src/dsp/drone.c`:

| Flag | 0: reference C | 1: optimized C |
|---|---|---|
| `OM_NOISE_CHUNK` | Six-state noise colour computed per sample | Stage an equivalent 16-sample run before the hot oscillator/filter loop |
| `OM_FILTER_SPECIALIZE` | General LP/BP/HP weighted sum per sample | Use one contribution when the filter-mode crossfade has settled |

The per-sample random-number progression and filter-state updates are
retained. The implementation comes from Niagara Drift's independently
authored, Frasil-informed commits, not copied Frasil code. Do not
include the old oscillator run-ahead or reciprocal BLEP shortcuts: the
previous headroom branch explicitly removed them after unsuccessful
benchmarking.

The Frasil-stage Timer4 profiler and executable-RAM code placement are
**not included in this first port**. They need separate dual-core
instrumentation and linker/XIP checks, especially since architecture C
already reserves `.c1_text` and `.c1_shared`. CPU1 GHOST/FX scheduling,
mailbox, and fault/OTA behavior remain unchanged.

## Host checks

```sh
sh tests/run_tests.sh
python3 tools/dsp_compare.py
# optional: needs valgrind
python3 tools/dsp_cost_variants.py --cases 3,4,5
OM_FX_CPU1=1 ./build.sh
sh tests/run_tests.sh
```

`dsp_compare.py` compiles reference `(0,0)` and optimized `(1,1)`
host renders with the same options, and requires bit-identical WAV files.
An identical host WAV is necessary, but hardware timing still determines
whether the optimizations are useful on the JieLi SoC.

For hardware A/B builds, all flags except the individual kernel toggles
must match. Keep each built firmware under a separate name:

```sh
OM_FX_CPU1=1 OM_NOISE_CHUNK=0 OM_FILTER_SPECIALIZE=0 ./build.sh
cp build/omni.fwsc build/architecture-c-kernels-reference.fwsc
OM_FX_CPU1=1 OM_NOISE_CHUNK=1 OM_FILTER_SPECIALIZE=1 ./build.sh
cp build/omni.fwsc build/architecture-c-kernels-optimized.fwsc
```

Record commit hash, STRESS V2 version, master, patch/transport state and
both CPU monitors (mean and maximum, late blocks, misses, faults).
Results must not be represented as faster until measured on hardware.
`main` remains unchanged pending cross-build, listening, long-run stress,
scene-transition, and OTA tests.
