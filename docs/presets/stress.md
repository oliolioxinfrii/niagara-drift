# CPU STRESS V2 — dynamic DSP benchmark patch

**Source:** `experiment/ghost14-ghost-fx-cpu1`

This scratch sound **replaces FUMAROLE** on the experimental branch. It is
loaded with **hold SEL, tap REC, release SEL**. PLAY starts audio. The
existing P1–P16 factory presets and user banks A–F are unchanged. The patch
is not automatically saved: reloading SEL+REC explicitly restores its
fixed parameters and four-step repeating pitch sequence. Ordinary REC
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
| Global movement | Movement 76; Drift, Age, Wander and noise off; Swell on | Continuous dual-channel envelope processing without random pitch wandering |
| Swell envelope | Rate 65, depth 58, shape Steps (102); Swing 25; two phase-offset envelopes with Wave 52, Skew -38, Edge 72, Width 65 | Evaluates the multi-stage envelope, interpolation, skew, edge and width branches |
| Sequencer | 4 notes/drone at 5 s each; 20 s cycle; 17% phase offset | Pitch interpolation and tuning updates over three complete cycles per 60 s |
| Gesture macro | WASH latched at 62 | Continuous macro envelope updates and dynamic FX coefficients |
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
2. Press **PLAY**, then **SEL+PLAY** to start the four-step pitch sequence.
   Confirm the sequence is running. This is essential: loading the preset
   alone prepares the sequence but does not start its transport.
3. Allow **12 seconds** for Shimmer to wash in and the FX to develop.
   Once started, do not change any controls during measurement.
4. On the same unit at the same master-volume setting, collect:
   `python tools/fm1_cpu.py 60` and
   `python tools/fm1_fx_cpu.py 60`.
   The CPU0 maximum in `fm1_cpu.py` is **since boot**, not limited to the
   current 60-second probe; note any earlier workload.
5. Record CPU0 mean and longest render, CPU1 mean and peak job time,
   late blocks, missed handoffs, faults, and audible artifacts.
6. Use the same patch, master volume, headphone/speaker routing, and
   GHOST MIX for every firmware revision. Firmware revisions that change
   the DSP algorithms can change the absolute load independently of
   the CPU architecture.

For long-run reliability, repeat on the same patch for 15–30 minutes
before treating the measured margins as production headroom. This V2 experiment is **not directly comparable** to the original STRESS
patch at commit `c3927ea`: Swell, WASH and a cycling sequencer add
work. It intentionally probes dynamic load rather than the most isolated
stationary DSP throughput. Additional host scripts or manual gestures
can test other transients, but changing knobs mid-run invalidates the
fixed-patch comparison.

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
