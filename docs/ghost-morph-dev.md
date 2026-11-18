# Architecture C: continuous GHOST timbral morph (development)

Branch: `dev/dualcore-integration`. The validated experimental baseline
`experiment/ghost14-ghost-fx-cpu1-headroom` and `main` are unchanged.

**SEL+PRESETS** now controls a 0–100 session-only **GHOST MORPH**:
0 = original **mode 5** (70% Cross-Synth + 30% Body);
100 = **mode 14** (Wave Terrain excites an eight-resonator Body).
The knob advances 2 percentage points per slow encoder detent, with the
standard encoder acceleration. A sample-smoothed ~75 ms sweep prevents
zipper noise.

**SEL+ALGORITHM** still controls **GHOST MIX**, i.e. how much the two
original parent drones contribute relative to the synthesized GHOST.
These are independent dimensions. Neither the GHOST MORPH nor the
diagnostic monitoring/muting switches change the saved project format.
SEL+REC STRESS sets the morph to mode 14's endpoint so earlier stress
measurements remain directly comparable.

## Algorithms

Mode 5 restores the original 12-band spectral envelope cross-synthesis
(two sets of 12 topology-preserving state-variable filters). Each parent
band is weighted by the other parent's measured band energy. The modal
body is driven by quadratic and cubic cross-products of the parents.
Individual Body, Cross-Synth, and final 70:30 output calibrations retain
the earlier experiment's slow gain laws and bounded cubic saturation.

Mode 14 uses the existing curved two-input Wave Terrain, whose mixed
finite difference removes both one-parent terms, into the existing
eight damped resonators. Its terrain math, modal tuning, gain and
post-GHOST compression are unchanged at morph 100.

Default **hybrid** (`OM_GHOST_BLEND_REFERENCE=0`): interpolate the body
*excitation* with equal-power coefficients, feed one shared eight-mode
resonator state, then progressively remove the 12-band Cross-Synth from
the mode-5 output as the terrain-driven output appears. Its final
normalization is interpolated at the output level: it is not guaranteed
to be perceptually constant-loudness at intermediate positions. The
middle is a single evolving resonant body plus the fading spectral
Cross-Synth, not two independent modal bodies.

A/B **reference** (`OM_GHOST_BLEND_REFERENCE=1`): preserve separate
mode-5 and mode-14 resonator banks and equal-power crossfade their
completed outputs. This retains mode 5 more literally for experiments
but uses more CPU1 and resonator state during intermediate morphs.

Both implementations use weights sqrt(1-t), sqrt(t), t in [0,1].
Thus the nominal amplitudes satisfy a²+b²=1, but because the signals
are correlated their combined RMS is not necessarily constant.
The common post-morph compressor remains in place. At the 100% endpoint,
the mode-5 spectral filters are bypassed and the mode-14 DSP is the same
as the validated baseline.

## Build and validate

```sh
git fetch origin
git switch dev/dualcore-integration
git pull --ff-only
sh tests/run_tests.sh
python3 tools/dsp_compare.py
OM_FX_CPU1=1 ./build.sh
sh tests/run_tests.sh
cp build/omni.fwsc build/niagara-c-ghost-morph-hybrid.fwsc

# A separate A/B firmware after recording the validated hybrid result:
OM_FX_CPU1=1 OM_GHOST_BLEND_REFERENCE=1 ./build.sh
cp build/omni.fwsc build/niagara-c-ghost-morph-crossfade.fwsc
```

`tests/scenarios/ghost-morph.omni` checks the knob's range, endpoints,
session semantics, unchanged GHOST MIX, and STRESS reset. The isolated
`ghost_mode5_test` checks Cross-Synth/Body excitation and state reset.

**Hardware caution:** Mode 5's additional 24 spectral-band filter
updates are expensive on CPU1. The previous mode-14+FX peak left
approximately 1.55 ms headroom in a 5.804 ms period; that is measured,
not a worst-case guarantee. Test the hybrid at morph 0, 25, 50, 75
and 100, both with modest FX and with STRESS-like heavy FX. Record
CPU0 longest, CPU1 mean/peak, misses and faults; stop if the deadline
is violated. The 15-minute STRESS V2 benchmark at morph 100 should
remain unchanged. Do not merge into main without hardware CPU1 data,
scene-transition testing, and listening/level review.
