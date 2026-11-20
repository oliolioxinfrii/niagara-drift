<!-- SPDX-License-Identifier: GPL-3.0-only -->
# GHOST final-voice candidate: 50% hybrid and TAIL return

**Branch:** `feature/ghost-voice-candidate` (forked from
`dev/dualcore-integration` at `683814d158b2fde7390441bbdb15684bc36de3ed`).
The generic continuous morph-development branch, original stable
`experiment/ghost14-ghost-fx-cpu1-headroom`, and `main` are preserved.

**Status: new sound-design candidate; FM-1 real-time stability unverified.**

## Sound / controls

- The GHOST timbre is **fixed at the development hybrid's 50% position**
  (equal-power source-dependent blend of Cross-Synth/Body and
  Wave-Terrain/Body). This uses the same eight-resonator hybrid bank as
  the previous development build, without a runtime timbre knob or slew.
- **SEL+PRESETS has no action.** PRESETS alone continues to adjust
  gesture MORPH TIME. The saved **GHOST MIX** (SEL+ALGORITHM), which
  balances direct parents against the GHOST voice, is unchanged and
  continues to participate in scene transitions.
- Legacy `dr_ghost_blend_set` and `dr_ghost_mode_set` entry points
  remain as inert compatibility stubs. The diagnostic blend getter is
  permanently 50; its old 5/14 classifier reports the nearest endpoint,
  not the true hybrid sound.
- STRESS retains its GHOST MIX target but no longer resets the fixed
  internal timbre. Existing key gestures, scenes and subtractive
  behavior are otherwise unchanged.

## TAIL return transient

Previously, TAIL used a single attenuation coefficient per 256-frame
block at the final parent mix and GHOST input. Returning from a fully
latched TAIL could introduce an abrupt gain step at an audio boundary.
The candidate applies a **50 ms bounded audio-rate gate** to each
drone's existing Swell gain instead. The identical per-sample gate
then reaches both the parent mix and GHOST's excitation (including
CPU1 source buffers). TAIL still leaves previously captured delay
and reverb energy circulating; no effect buffers are cleared.

This is a targeted **dezippering hypothesis** for the reported release
noise. It must be evaluated by listening on hardware, especially with
strong FEEDBACK, GHOST MIX and long or infinite reverb.

## Validation

Host tests should confirm that the fixed 50% GHOST remains audible,
diagnostic setters cannot change it, SEL+PRESETS is inert, and TAIL
returns with a gradual direct-source onset. They do **not** validate
the dual-core handoff deadline.

To build this candidate in Codespaces, after reviewing the source
and using a clean working tree:

```sh
git fetch origin
git switch feature/ghost-voice-candidate
git pull --ff-only
sh tests/run_tests.sh
OM_FX_CPU1=1 ./build.sh
sh tests/run_tests.sh
cp build/omni.fwsc build/ghost-voice-50-candidate-UNVALIDATED.fwsc
```

Do not replace a known-good package. On a fresh full power cycle,
load STRESS V2 and monitor `python tools/fm1_fx_cpu.py 60` immediately
after starting playback. The earlier generalized hybrid at 50%
**retired CPU1 after one missed handoff**; the candidate may also
exceed the 5804-us deadline. The old completed-job peak did not
measure the unfinished offending block. Zero misses/faults over a
short trial would not establish long-term stability.

The new four-key excitation effects are **out of scope** pending
separate discussion and acceptance of this candidate.

## November 19 hardware feedback and recovery experiment

The fixed midpoint plays on the FM-1, but CPU1 processing is close to its
deadline. Initial Niagara: CPU1 completed-job mean 5184 us, peak 5738 us
against 5804 us per block, with no missed handoffs during the probe.
STRESS V2 caused one missed handoff and retired CPU1. After a power
cycle Niagara ran again: mean 5215 us, peak 5573 us, no misses during
that probe. These are observed probe values, not real-time guarantees;
the missed/incomplete job is excluded from completed-job timing.

The 50-ms TAIL source dezipper did not eliminate the reported return
boom. A new hypothesis is a strong resonator/cross-synthesis response
when a silent parent is reintroduced. This revision adds a **GHOST-only**
recovery envelope: when either TAIL reaches approximately 100%, GHOST
direct output fades out over 20 ms. After both return, its output
remains silent for 100 ms, then rises over 150 ms. The input and
resonator DSP continues processing so its transient settles inaudibly;
the existing parent TAIL fade and the shared delay/reverb contents
are unaffected. This has not yet been verified by hardware listening.

Occasional crackles while moving MASTER may originate from block-wide
gain changes. The candidate now low-pass-filters both master coefficients
at 25 ms per block and linearly interpolates them sample-by-sample
inside the output loop; the settled mapping and clipping are unchanged.
This is also a hypothesis to validate on hardware.

**Neither change solves the CPU1 stress regression.** Reprofile CPU1
and keep the validated mode-14 build available. No excitation gesture
work is included.


## Historical CPU0 Cross-Synth split and resonator-excitation fade (reverted)

This revision keeps the **fixed 50% hybrid timbre** and relocates the
24-band Cross-Synth signal calculation to CPU0. The two cores no longer
share live Cross-Synth filter state:

- CPU0 renders both parent drones as before, and computes the mode-5
  Cross-Synth output using its own two-pole input smoothing and filter
  bank. When GHOST is inactive, CPU0 resets only this private state.
- The existing double-buffered mailbox gains a **256-sample mono
  Cross-Synth waveform per slot** (1,024 bytes/slot).
- CPU1 continues independently smoothing the same parent samples,
  generating the mode-5 body excitation and Wave Terrain, exciting the
  **single shared eight-mode resonant body**, applying GHOST's adaptive
  gain/compression, and rendering shared delay and reverb.
- CPU1 never modifies CPU0's Cross-Synth band or low-pass states.
  The existing shared-slot compile-time memory limit remains enforced.
  An identical host renderer is retained as the single-core reference.

The old TAIL recovery envelope hid the onset of GHOST's final output
but did not limit the new energy injected into the resonator. The new
envelope continues to hold GHOST output silent for 100 ms after
releasing a fully engaged TAIL. After that hold, the **actual sum of the
mode-5 body excitation and terrain excitation** is increased smoothly
from 0 to full over **350 ms**, before injection into the resonant
body. The separate 150-ms output fade is retained, and previous reverb
and delay contents continue ringing.

**Important:** This is a workload *redistribution*, not yet a verified
performance improvement. The hardware has previously shown a 5.7-ms
CPU1 completed-job peak (5.804-ms deadline), and CPU0's STRESS peak
was 4.164 ms. The additional work on CPU0 might produce CPU0 audio
overruns even if CPU1 now runs comfortably. Both CPU monitors must be
observed on a fully power-cycled FM-1, especially under STRESS V2.
A complete host-test pass does not verify two-core scheduling.

The firmware-update quiescence and sticky CPU1 retirement mechanisms
are **unchanged**. This is not an automatic-restart experiment.

### Rebuild and hardware evaluation

```sh
git fetch origin
git switch feature/ghost-voice-candidate
git pull --ff-only
sh tests/run_tests.sh &&
OM_FX_CPU1=1 ./build.sh &&
sh tests/run_tests.sh &&
cp build/omni.fwsc build/ghost-voice-cpu0-cross-tail-ramp-TEST.fwsc
```

Test the regular Niagara GHOST 50% sound, full TAIL release, then STRESS
V2, recording CPU0 average/peak/late and CPU1 online/mean/peak/misses
after each scenario. Compare the **sound** to the previous
`c2304fb` candidate, and the timing to its 5.7-ms CPU1 peak.


## CPU1 Cross-Synth restored (November 19, 2026)

The CPU0 Cross-Synth split above was **reverted** on this branch after
FM-1 measurements showed CPU0 at 102.8% under STRESS V2 (5,165 late
audio blocks), despite reducing CPU1's completed-job time. The
historical section above documents an abandoned experiment, not the
current firmware architecture.

The dual-core renderer again computes the complete GHOST fixed 50%
hybrid, including the 24-band Cross-Synth, Wave Terrain, eight-mode
resonant body, adaptive output and shared FX, on CPU1. CPU0 renders
the parent sources and sends aligned source blocks and controls through
the original mailbox; no Cross-Synth waveform or filter state remains
on CPU0. The original Cross-Synth reset and denormal flush behavior is
restored on CPU1.

The later **350-ms resonator-excitation ramp after the 100-ms TAIL hold**
and the host-only regression test are retained. The 150-ms GHOST output
fade and the earlier parent-source TAIL dezipper also remain in place.

This is a return to the previous division of labor, **not a stability
qualification**: CPU1 already exceeded its real-time handoff deadline
under STRESS V2 before the CPU0 experiment. Ordinary Niagara loads may
play, but long-run stress stability remains unverified.

Rebuild and remeasure this branch after a clean power cycle:

```sh
git fetch origin
git switch feature/ghost-voice-candidate
git pull --ff-only
sh tests/run_tests.sh &&
OM_FX_CPU1=1 ./build.sh &&
sh tests/run_tests.sh &&
cp build/omni.fwsc build/ghost-voice-cpu1-cross-tail-ramp-TEST.fwsc
```

Record CPU0 average/peak/late and CPU1 mean/peak/misses/faults
separately under Niagara and STRESS V2. Neither a host test pass nor
the absence of misses in a brief Niagara run establishes stable
real-time performance.
