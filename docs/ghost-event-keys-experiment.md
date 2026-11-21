<!-- SPDX-License-Identifier: GPL-3.0-only -->
# GHOST excitation-key experiment (half-rate hybrid)

**Branch:** `experiment/ghost-excitation-keys` (from
`experiment/ghost-cross-half-rate-22050`, baseline `f90dc3a`).

**Status:** Source and host-test changes committed. Host suite and physical
FM-1 real-time performance **have not been run for this revision**.

## Changes

1. **SEL+OCT− / SEL+OCT+** still toggles the corresponding parent drone.
   Its direct-output gain now rises/falls over **MORPH TIME** (`P_MORPH`),
   like the twelve held-depth keys. The overall PLAY transport continues to
   use the separate **FADE** time (`P_FADE`). The MORPH-driven gain step is
   cached when MORPH changes, rather than calculating its power curve in the
   audio hot loop.
2. **TAIL → GHOST recovery is retained unchanged** from the tested half-rate
   baseline: a fully closed TAIL attenuates GHOST output and resonator input;
   release holds GHOST silent for 100 ms, then ramps the **resonator
   excitation** over 350 ms while direct GHOST output returns over 150 ms.
   Reverb and delay tails are unaffected.
3. **Keys 13–16** (D5, E5, F5, G5) replace the previous
   WEIGHT/SHIFT/CLEAN/WASH physical-key bindings with four event triggers:
   - 13 **STRIKE**: 15 ms shaped impulse; repeats every 300 ms while held.
   - 14 **SCRAPE**: 250 ms irregular noise excitation; 650 ms repeat.
   - 15 **BOW**: 1.6 s smooth noise-driven swell; 1600 ms repeat.
   - 16 **TREMOR**: five irregular, alternately signed pulses biased toward
     the lower four resonators; 900 ms repeat.

A press triggers the event once. Holding schedules additional triggers, and
release stops retriggering without clearing the ringing resonators. Events
have bounded gain and excite **the existing eight-mode bank**, rather than
adding independent voices. No additional controls or stored project
parameters were added. Existing historical macro values in saved projects
remain readable; only their physical key assignments were replaced.

### Event loudness correction (2026-11-20)

The first hardware installation displayed the new event popups but the four
sounds were not perceptible. Source inspection showed the modal bank injects
only approximately 0.0002-0.0004 of an excitation per sample; the first
event envelopes (0.15-0.65) were not calibrated for this coefficient.
The branch therefore now applies isolated STRIKE/SCRAPE/BOW/TREMOR gains of
8/48/30/12 before bounded injection, without changing parent excitation,
resonator tuning, or the no-event DSP path. This is a **provisional
calibration**, not verified loudness or a proven resolution of hardware
delivery. The host audio test now requires each event to produce both
an absolute output RMS difference of over 0.001 and over 2.5% of the
reference signal, not merely a nonzero difference (previously 0.000001).

If still inaudible after reflashing, first verify GHOST MIX 100, both
parent drones active, and neither TAIL fully engaged. If those hold and
the host difference test passes, the remaining suspect is delivery along
the hardware-only CPU0->CPU1 event mailbox rather than the host DSP itself.
Do not claim a CPU1 handoff has been verified by the host-only test.

## Architecture

UI events are queued as `C_GHOST_EXC` commands. CPU0 drains the command
queue and packs a four-bit pending event mask into the next CPU1 shared
snapshot (`fx_c1_slot_t.ghost_events`). Only CPU1 launches and advances
the excitation state. When GHOST is inactive, events are discarded with
the normal GHOST reset. The original resonator hot path is retained for
samples without active events; there is no extra resonator bank, oscillator,
cross-core wait or control-plane memory allocation.

When fully closed by TAIL, the common 350-ms excitation gate also suppresses
new event energy. Multiple different events can overlap; repeating one
event restarts its envelope but leaves resonant energy intact.

## Validation

The half-rate baseline completed 15 minutes of STRESS V2 at GHOST MIX 50
with no new missed handoffs or CPU1 faults (CPU1 recent mean 5054 us,
recorded completed-job peak 5728 us versus 5804-us budget). This revision
has **not** demonstrated that stability. Its 76-us prior peak margin is
narrow: run CPU diagnostics with and without GHOST events.

From the home WSL checkout, after protecting any uncommitted work:

```sh
cd ~/niagara-drift
git fetch origin
git switch --track origin/experiment/ghost-excitation-keys
git rev-parse --short HEAD
sh tests/run_tests.sh
OM_FX_CPU1=1 ./build.sh
sh tests/run_tests.sh
cp build/omni.fwsc build/ghost-event-keys-half-rate-EXPERIMENT.fwsc
```

If that local branch already exists, switch it normally and fast-forward
instead. Do not flash if any test or firmware build fails.

To install from WSL using **Windows Python** (native WSL ALSA MIDI is absent
on the user's home PC), after saving a backup and keeping the known-good
half-rate `.fwsc`:

```sh
WIN_INSTALLER=$(wslpath -w "$PWD/tools/fm1_install.py")
WIN_PACKAGE=$(wslpath -w "$PWD/build/ghost-event-keys-half-rate-EXPERIMENT.fwsc")
python.exe "$WIN_INSTALLER" --info
python.exe "$WIN_INSTALLER" --backup
python.exe "$WIN_INSTALLER" "$WIN_PACKAGE"
python.exe "$WIN_INSTALLER" --info
```

After a fresh power cycle, with Niagara GHOST MIX 50 and then STRESS V2:

```sh
WIN_CPU1=$(wslpath -w "$PWD/tools/fm1_fx_cpu.py")
WIN_CPU0=$(wslpath -w "$PWD/tools/fm1_cpu.py")
python.exe "$WIN_CPU1" 60
python.exe "$WIN_CPU0" 60
```

Check for changes in CPU1 headroom when no event is pressed and under
single and overlapping held key events; check CPU0 lateness and headphone
sound. Exercise TAIL 100% release, GHOST MIX 0/50/100, repeated event
taps, and ordinary scene/sequence play.

**The experiment is not a qualified stable firmware until physical
tests pass.**
