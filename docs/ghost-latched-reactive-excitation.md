<!-- SPDX-License-Identifier: GPL-3.0-only -->
# GHOST reactive excitation and session latch experiment

**Branch:** `experiment/ghost-latched-reactive-excitation`

**Base:** `experiment/ghost-excitation-keys` (the previously flashed,
audible-but-quiet experimental version). Both the half-rate Cross-Synth
and the existing GHOST TAIL 100-ms dead zone / 350-ms excitation ramp
remain in place.

## Controls (keys 13–16)

- **Single press:** one immediate excitation; no double-tap latency
- **Hold:** automatically retrigger at the event's period, until released
- **Double-tap:** two presses shorter than 220 ms each (second press
  begins within 320 ms of first release); the second press enables
  a repeating **latch** that survives release
- **Press again while latched:** cancel that latch without triggering
  another event; existing resonator tails ring out naturally

Each key latches independently. Latched keys show a slowly pulsing
LED; a currently held unlatched key stays bright. These are session-only
performance states and are not written into the saved sound. In
sequence-note entry, keys continue entering notes and existing latches
remain active; a released held key cannot get stuck repeating when the
page changes. The SAVE menu likewise releases an existing held key.

## DSP changes

| Key | Event | What changes with the sound | Repeat |
|---|---|---|---|
| 13 | STRIKE | Stronger 15-ms single impact into all 8 resonators | 300 ms |
| 14 | SCRAPE | 250-ms friction between the *actual parent audio waves*: product-sidebands and differential-phase shear, high-passed and injected mainly into the four upper resonators; **no independent noise** | 420 ms |
| 15 | BOW | 1.6-second slow-rise tonal coupling of the two parents' current audio into the resonators. Excitation depends on both source RMS-power estimates; **no independent noise** | 1600 ms |
| 16 | TREMOR | Stronger, seven-impact irregular, alternating-sign low-mode cluster with a smaller parent-responsive upper-partial component | 900 ms |

These still excite only the existing eight-mode GHOST resonator bank.
Events restart their envelopes on retrigger, but do not reset the resonator
states. Multiple event types may overlap. Event injections are bounded.
The previous unmodified resonator hot path is retained for samples with no
events active, and no new oscillator or independent FX channel is created.

The existing hybrid's parent DSP, half-rate Cross-Synth, compressor,
GHOST MIX, and shared delay/reverb are otherwise unmodified.

## Validation required

**Source and tests are committed but have not been built or run in this
assistant environment.** The prior STRESS V2 half-rate reference achieved
5,728 µs recorded CPU1 peak against a 5,804 µs block budget: only
76 µs headroom. Performance must be remeasured when the events are
**latched and overlapping**. Do not infer stability from idle playback.

Host test coverage:

- `tests/scenarios/ghost-excitation-keys.omni`: single press, hold,
  double-tap latch, independent simultaneous latches, cancellation,
  cancel does not trigger a new event, held keys during modal UI
- `tests/host/ghost_events_test.c`: isolated injected-energy tests
  proving SCRAPE and BOW depend on both parent source signals
- `tests/host/drone_test.c`: full-GHOST audibility and Morph Time
- All older project, macro persistence, scenario and firmware tests

WSL build:

```sh
cd ~/niagara-drift
git status -sb
git fetch origin
git switch --track origin/experiment/ghost-latched-reactive-excitation
sh tests/run_tests.sh
OM_FX_CPU1=1 ./build.sh
sh tests/run_tests.sh
cp build/omni.fwsc build/ghost-reactive-latch-EXPERIMENT.fwsc
```

Do not flash if any tests fail. Keep the previous working firmware
package available for rollback. After a successful build, use the same
Windows Python FM-1 installer from WSL:

```sh
WIN_INSTALLER=$(wslpath -w "$PWD/tools/fm1_install.py")
WIN_PACKAGE=$(wslpath -w "$PWD/build/ghost-reactive-latch-EXPERIMENT.fwsc")
python.exe "$WIN_INSTALLER" --info
python.exe "$WIN_INSTALLER" --backup
python.exe "$WIN_INSTALLER" "$WIN_PACKAGE"
python.exe "$WIN_INSTALLER" --info
```

Suggested hardware sequence: GHOST MIX 100 with both drones present
and neither TAIL engaged, first compare single STRIKE/TREMOR and their
overlap, then SCRAPE/BOW while varying both parent waveforms, intervals,
filter settings, and levels; double-tap each latch and cancel. Observe
the CPU1 diagnostic with *two or more latches active* at GHOST MIX 50,
then test STRESS V2. Treat a single missed handoff or retired CPU1 as
a failure of this version.
