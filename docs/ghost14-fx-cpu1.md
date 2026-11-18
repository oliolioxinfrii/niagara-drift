<!-- SPDX-License-Identifier: GPL-3.0-only -->
# Mode 14: source on CPU0, shared FX on CPU1

**Branch:** `experiment/ghost14-fx-cpu1` (separate from the existing
`experiment/ghost-mode14-dualcore`; the former implementation is unchanged).

**STATUS: SOURCE-ONLY, UNBUILT, UNVALIDATED, DO NOT INSTALL ON HARDWARE.**
This branch starts from the prior CPU1 feasibility experiment so that the
proven CPU1 mailbox, fault retirement, GHOST synthesis and update shutdown
can be reused. Unlike stable `main`, it **does not yet include the 19
headroom-optimization commits**. Integrating those is a subsequent requirement
before a meaningful benchmark against stable headroom firmware.

## Architecture

- CPU0 owns both parent drones, their modulation and cross-interactions,
  and **serial** GHOST mode 14 (Wave Terrain -> modal body).
- CPU1 owns the delay/reverb/drive state in `drone_fx.c` and processes a
  complete **256-sample stereo** block. GHOST and the parents arrive at FX
  aligned, with no GHOST-only delay.
- Two 256-frame shared-SRAM slots alternate: CPU0 fills the newly free slot
  while CPU1 processes the previously prepared slot. Each slot carries
  immutable copies of `par`, `pfs`, the 16-sample master-LFO ticks, fade
  and external clock rate. CPU1 alone writes the FX state.
- On each audio interrupt CPU0 reaps the previous CPU1 job *without waiting*,
  submits the prior prepared block, synthesizes the next source block, and
  plays the completed FX block two source intervals behind (approx. **11.6 ms
  additional scheduling latency**). The output is delayed as a whole.
- CPU0's FEEDBACK and SNARL ring receives the **older post-effects audio**.
  This necessarily alters feedback phase and effective delay, especially
  when the loop period is 256-510 samples. It is **not audio-equivalent**
  to the single-core engine. The loop must be tested musically and for
  unstable feedback under worst-case patches.
- On CPU1 timeout/fault the platform holds/retires CPU1; CPU0 bypasses FX
  rather than waiting or executing the heavy FX path. The next audio block
  continues with parent/GHOST sources, but there may be a discontinuity.
  CPU1 faults are reported by the diagnostic command, not silently ignored.
- A pending panic requests FX-state reset through the next job; CPU0 never
  clears active CPU1 FX memory. During OTA and USB UBOOT the platform holds
  CPU1, marks it offline for flash access and unbinds the FX dispatcher.

## Build and diagnostic commands (once code review/build checks pass)

```sh
tests/run_tests.sh
OM_FX_CPU1=1 ./build.sh
cp build/omni.fwsc build/ghost14-fx-cpu1-UNVALIDATED.fwsc
```

```sh
python3 tools/fm1_cpu.py 60        # CPU0 render cost
python3 tools/fm1_fx_cpu.py 60     # CPU1 FX job latency, misses, faults
```

**Do not mistake a successful host build for hardware approval.** Before
flashing: cross-compile with the JieLi toolchain, confirm .c1_shared and CPU1
stack margins, inspect disassembly and flash-XIP behavior, and review
producer/consumer publication fences, ISR timing and firmware-update paths.
A fresh dry-only / GHOST / effects cost comparison is required after the
missing stable headroom optimizations are brought over. Then validate output
routing, delay tails, panic, GHOST levels, MIDI-clock modulation, global
FEEDBACK/SNARL, fault fallback, and OTA while CPU1 is busy.

## Historical comparison

The earlier `experiment/ghost-mode14-dualcore` kept FX on CPU0 and ran
GHOST on CPU1, delayed by 256 samples relative to the dry parents. That
implementation is retained as the direct comparison and recovery reference.
