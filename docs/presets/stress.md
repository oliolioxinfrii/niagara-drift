# CPU STRESS — deterministic DSP benchmark patch

**Source:** `experiment/ghost14-ghost-fx-cpu1`

This scratch sound **replaces FUMAROLE** on the experimental branch. It is
loaded with **hold SEL, tap REC, release SEL**. PLAY starts audio. The
existing P1–P16 factory presets and user banks A–F are unchanged. The patch
is not automatically saved: reloading SEL+REC explicitly restores its
fixed parameter values and one-step stationary sequence. Ordinary REC
continues to operate Motion Loop.

The original FUMAROLE experiment remains available in Git history and
earlier branches; its hardware shortcut is repurposed here.

## Fixed DSP workload

| Section | Fixed configuration | Purpose |
|---|---|---|
| Parent I | D3, complex X-Wave, detuned just-fifth partner, subharmonic, LP filter, resonance, filter drive | Continuous full voice chain |
| Parent II | A3, distinct X-Wave, detuned major-third partner, subharmonic, BP filter, resonance, drive | Second simultaneous full voice chain |
| Coupling | Cross-FM at 48; hard sync on; pitch link off | Time-dependent cross-drone work |
| Modulation | Both oscillator LFOs move X-Wave; filter LFOs modulate cutoff and resonance; periodic LFO shapes | Parameter modulation without random shape changes |
| Global movement | Movement 76; Drift, Age, Wander and noise disabled; Swell off | Stable excitation without stochastic pitch wandering |
| GHOST | Fixed mode 14, MIX 50, unmuted | Continuous CPU1 terrain + modal synthesis |
| Delay | Grain, time 92, repeats 64, wet 55, LFO both, depth 50 | Stereo delay and modulation work |
| Post-delay | Drive 56; route Both | Drives downstream waveshaping |
| Reverb | Shimmer, mix 65, length 86, damp 30 | High-load reverb with octave shifter |
| Global feedback | 40 | Exercises post-FX to parent feedback without maximum runaway |

All values are **knob/parameter units**, not calibrated signal levels. The
preset is a deterministic *control configuration*, not an assertion that
every internal sample will be bit-identical after an arbitrary warm start:
effect tails, previous phase state, and startup timing can still differ.

## Repeatable hardware procedure

1. Boot the candidate firmware and load **SEL+REC → CPU STRESS**.
2. Press PLAY and allow **12 seconds** for the Shimmer wash-in and
   the delay/reverb to settle. Do not change any controls thereafter.
3. On the same unit at the same master-volume setting, collect:
   `python tools/fm1_cpu.py 60` and
   `python tools/fm1_fx_cpu.py 60`.
   The CPU0 maximum in `fm1_cpu.py` is **since boot**, not limited to the
   current 60-second probe; note any earlier workload.
4. Record CPU0 mean and longest render, CPU1 mean and peak job time,
   late blocks, missed handoffs, faults, and audible artifacts.
5. Use the same patch, master volume, headphone/speaker routing, and
   GHOST MIX for every firmware revision. Firmware revisions that change
   the DSP algorithms can change the absolute load independently of
   the CPU architecture.

For long-run reliability, repeat on the same patch for 15–30 minutes
before treating the measured margins as production headroom. Gradual
live-parameter stress tests are separate experiments: changing the
knobs invalidates a fixed-patch comparison.

The scratch preset exercises processing pathways but does **not** prove
a worst-case execution-time bound. The associated 60-second host render
is `examples/stress-60s.omni` (produces `stress-60s.wav` in the
specified host output directory); host builds do not execute the physical
CPU1 mailbox.

## Firmware and host validation

```sh
sh tests/run_tests.sh
OM_FX_CPU1=1 ./build.sh
sh tests/run_tests.sh
cp build/omni.fwsc build/ghost14-ghost-fx-cpu1-stress-UNVALIDATED.fwsc
```

No firmware package should be flashed based on these source edits alone.
