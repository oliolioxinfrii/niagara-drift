<!-- SPDX-License-Identifier: GPL-3.0-only -->
# GHOST mode 14: CPU1 feasibility experiment

**Branch:** `experiment/ghost-mode14-dualcore`.

**Status: experimental source, not validated for hardware installation.** Derived from
`feature/volcano-ghost-intermodulation` at `66b8f176`. Stable `main` and the
full 15-mode GHOST branch are untouched.

## Scope

Only **TERRAIN -> BODY** (formerly GHOST mode 14) is compiled into the
active DSP: a nonlinear two-input wave terrain excites eight damped
modal resonators. The original parent drones, their filters, cross-FM,
feedback, shared delay, drive and reverb remain. The mode selector is fixed
to numeric mode 14 for compatibility. Other experimental GHOST algorithms
are excluded from the compiled engine and removed from this branch.

## CPU1 proof-of-concept design

The 4,112-byte CPU1 audio mailbox payload is allocated in dedicated
`.c1_shared` high SRAM, after persistent `.noinit` records and below the
reserved boot/vector region. Firmware startup clears this transient section.
This avoids exhausting the ordinary DSP/effects `.pool` headroom while
preserving its 8-KiB minimum safety margin.

The `OM_GHOST_CPU1=1` build (opt-in) enables the guarded CPU1 worker
adapted from Arctic Scream / felucca-bsp, with a checksum mailbox,
dedicated stack, fault retirement, startup guard and flash-XIP safety.

CPU0 renders both parents and publishes a 256-sample float32 block from
each plus parameter snapshots to shared SRAM. CPU1 runs the mode-14
terrain and resonators and writes 256 mono GHOST samples. CPU0 consumes
the previous completed GHOST block, adds it before the shared effects,
and submits the next block. CPU0 never waits for a busy CPU1 inside
the audio interrupt; it counts missed handoffs and continues without
GHOST for that block. On CPU1 failure the original parent drones continue.

**Important:** this throughput probe deliberately delays GHOST by
**256 samples (~5.805 ms)** relative to the current parent audio.
That changes the phase relation and feedback excitation. It is not
audio-equivalent to the serial reference, and not a finished architecture.

## WSL build procedure

```sh
git fetch origin
git switch -c mode14-cpu1-test origin/experiment/ghost-mode14-dualcore
tests/run_tests.sh
./build.sh
cp build/omni.fwsc build/ghost14-serial.fwsc
OM_GHOST_CPU1=1 ./build.sh
cp build/omni.fwsc build/ghost14-cpu1-UNVALIDATED.fwsc
```

The host tests exercise the serial DSP and UI. The CPU1 path is platform
assembly and must independently pass the JieLi linker, RAM-budget checks
and a device-specific review. **Do not flash the unvalidated dual-core
package merely because it compiles.** Preserve a known-good main firmware
package and a recovery method.

## On-device measurements (after hardware validation)

```sh
python3 tools/fm1_cpu.py 15         # CPU0: longest render, late DMA halves
python3 tools/fm1_ghost_cpu.py 15   # CPU1: mean/peak time, misses, faults
```

Compare the serial and CPU1 builds with GHOST mix 0, 50 and 100
and the maximal parent workload. Success requires CPU1 processing
comfortably below the 5805-us deadline, zero missed handoffs, zero
CPU1 faults, and no new CPU0 audio dropouts. A successful throughput
probe must still be redesigned to maintain correct phase alignment.

## Relationship with the stable headroom work

This proof-of-concept inherits the **GHOST branch**, not the 19
headroom commits later merged into `main`. The successful noise
staging, filter specialization, RAM-code path and panel/delay fixes
are not automatically included. The stable firmware remains the
baseline to reconcile after feasibility is established.

## Credit

The CPU1 mailbox/startup and exception handling were adapted from
[Arctic Scream](https://github.com/PolarPatch/Arctic-scream-FM-1)
and [felucca-bsp](https://github.com/hugelton/felucca-bsp).
Original MPL-2.0 provenance is preserved in the imported files and
`LICENSING.md`.
