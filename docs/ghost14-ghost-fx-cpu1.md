<!-- SPDX-License-Identifier: GPL-3.0-only -->
# Experiment C: CPU0 parent drones, CPU1 GHOST mode 14 + shared FX

**Branch:** `experiment/ghost14-ghost-fx-cpu1`
**Build flag:** `OM_FX_CPU1=1` (the same worker/diagnostic protocol as experiment B).

**STATUS: new source experiment; cross-compile and hardware measurements not yet verified.**
Do not confuse the creation of this GitHub branch with a tested firmware package.
Preserved reference branches:
- `experiment/ghost-mode14-dualcore` — CPU1 GHOST only; CPU0 parents+FX
- `experiment/ghost14-fx-cpu1` — CPU0 parents+GHOST; CPU1 FX

## Scheduling and signal path

CPU0 renders **both parent drones**, then publishes one immutable 256-frame
job. CPU1 processes **GHOST mode 14** from the separate parent excitations,
mixes GHOST and the corresponding stereo parent block, and runs the entire
shared FX graph on that combined signal. CPU0 later consumes the
already-processed stereo output for FEEDBACK, speaker filtering and DMA.

```text
CPU0: parent I + II --> independent parent excitation + faded parent stereo
                               | (one immutable job, shared SRAM)
CPU1:                        mode-14 GHOST
                                | (same-block alignment)
                            parent/GHOST mix
                                |
                             shared FX
                                | (completed stereo block)
CPU0:                     FEEDBACK / output
```

The two alternating jobs each carry: faded stereo parent signal; two separate
parent-waveform excitation blocks, premultiplied by their original per-sample
gain and subsystem gains; faded GHOST wet coefficient for each sample;
per-block parent pitches, GHOST mute/active state; and the existing full
`par`/`pfs` control snapshot, master-LFO ticks, fade, and clock rate.

The CPU1 worker alone owns GHOST resonators and compressor **as well as**
delay, drive and reverb state. The CPU0 ISR does not block waiting for it:
at the start of each period it reaps the last completion, dispatches the
previously prepared job, renders and prepares the next block, and plays the
prior completion. Startup has two priming intervals of silence. At 44.1 kHz,
256 frames = ~5.805 ms: overall scheduling latency is **two intervals
(~11.6 ms)**, equal to experiment B. Relative parent/GHOST latency is zero.

FEEDBACK/SNARL still receives an output that is older than the
single-core reference by these pipeline intervals. Even if parent/GHOST
phase alignment is preserved, the global feedback response may change.
Use careful listening tests with strong FEEDBACK and SNARL settings.

If CPU1 is offline or retired after an overrun/fault, CPU0 retains the
direct *parent* signal at its original full mix even with GHOST MIX at 100
(no GHOST or shared FX fallback). This is deliberately different from
experiment B's local-GHOST fallback. A discontinuity may occur.
CPU1 startup-failure guard, flash-XIP exclusion, and the OTA/UBOOT CPU1
shutdown sequence are inherited unchanged from experiment B.

## Build / test (on a machine with the JieLi toolchain and AC79 SDK)

```sh
git fetch origin
git switch --track origin/experiment/ghost14-ghost-fx-cpu1
sh tests/run_tests.sh
OM_FX_CPU1=1 ./build.sh
sh tests/run_tests.sh  # also exercises packaged-firmware update tests
cp build/omni.fwsc build/ghost14-ghost-fx-cpu1-UNVALIDATED.fwsc
```

Do **not** flash based solely on a host build. Review linked memory margins,
cross-core synchronization, failover, and the completed firmware package first.
Then test gradual GHOST levels, effects, feedback and OTA on the hardware.
The 0x7D CPU1 diagnostic now times **both GHOST and FX**, not just FX:

```sh
python3 tools/fm1_cpu.py 60
python3 tools/fm1_fx_cpu.py 60
```

## Comparison and known limitations

Experiment B (CPU0 parents+GHOST, CPU1 FX) previously ran at GHOST MIX 50,
with no reported auditory discrepancy, with CPU0 average 86% and recorded peak
94.4%; CPU1 FX average 1817 us, peak 1920 us; zero CPU0 late blocks, zero
CPU1 handoff misses, zero CPU1 faults in the 60-second observations.

This **does not predict** C's CPU1 load. GHOST mode 14 and FX have sequential
dependency inside each CPU1 job; their combined worst-case time, rather than
their averages, must stay within the 5804 us DMA block period. CPU0 now
publishes three additional float arrays and may incur extra memory-transfer
cost. The firmware still lacks the stable `main` optimization commits
present when experiment B was branched; do not compare to `main` as though
they are at identical revisions.

The normal host tests compile the mono reference path (`OM_HOST`); they do
not simulate the hardware CPU1 mailbox, XIP flash gates, or parallel producer
and consumer. Cross-compilation and hardware validation are mandatory.
