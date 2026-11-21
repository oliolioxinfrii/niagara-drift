<!-- SPDX-License-Identifier: GPL-3.0-only -->
# GHOST excitation audibility in MIX 25–50

**Experimental branch:** `experiment/ghost-event-audibility-mix50`

**Base:** `experiment/ghost-latched-reactive-excitation`. This
branch preserves the half-rate Cross-Synth (22.05 kHz), four
parent-responsive excitation events, independent double-tap latches,
MORPH-timed SEL+OCT, and existing GHOST TAIL recovery.

## Reason for the revision

Hardware listening reported that SCRAPE became musically more
interesting, but BOW was nearly inaudible, and all four excitation
effects were masked by the two parent drones when GHOST MIX was at
50 or below. Boosting only the resonator injection failed to address
two downstream gain stages: GHOST compression and wet/dry mixing.

## Audio and CPU changes

1. **Post-compressor GHOST event accent:** a scalar attack/release
   envelope rises in approximately 4 ms while any excitation event
   runs and decays over approximately 0.5 s after the last active
   event. It multiplies the GHOST output **after its compressor** but
   **before** the GHOST MIX wet coefficient, the GHOST mute and the
   TAIL output gate. It does not change the parent drones or modify
   saved sound parameters.

2. **Mix-sensitive gain:** the accent is stronger at lower GHOST MIX
   settings. At full accent, gain relative to the unaccented GHOST bus
   is approximately:
   - MIX 100: 1.8× (+5.1 dB)
   - MIX 50: 3.32× (+10.4 dB)
   - MIX 25: 4.4× (+12.9 dB)
   - MIX 10: 5.2× (+14.4 dB)
   - MIX 0: **silent**, since the existing wet coefficient is
     unchanged and zero at MIX 0.

   Note: This boosts the **whole shared GHOST resonator output while
   an event is active**, not a mathematically isolated per-event signal.
   Since the four event excitations share the same eight-resonator
   state, extracting their individual ringing tails would otherwise
   require extra resonator/filter state and CPU1 work. The behavior is
   intentionally temporary and fully returns to unity after release.

3. **BOW source gating:** replace
   `clamp(2500 * power0 * power1, 0, 1)` with
   `clamp(800 * min(power0, power1), 0, 1)` and
   raise BOW input coupling 5→11. With a typical small parent
   power of 0.002 the former gate was only 0.01; the new gate
   reaches unity, but still requires **both** actual parent sources.
   BOW remains a 1.6-second tonal excitation of the existing
   resonator bank; no independent FX oscillator or noise path.

4. **Real-time budget:** only a scalar envelope and a few arithmetic
   operations are added to each audible GHOST sample. The original
   no-event resonator path is unchanged and the accent helper exits
   immediately while idle. CPU1's previously observed worst-case
   headroom was only 76 µs, so hardware testing is mandatory before
   using multiple latches in a performance.

## Tests

The host tests now cover:
- Event accent over 10 dB at MIX 50, larger boost at MIX 25,
  strict MIX 0 silence, and unity recovery after the resonator tail
- Full two-drone audio output A/B for STRIKE, SCRAPE, BOW and
  TREMOR at GHOST MIX 25 and 50 (each must change the signal)
- Existing excitation source-dependency, UI double-tap latching
  and legacy macro persistence tests

**These new tests have not been executed by the assistant.**

## Build and hardware trial

Keep the existing playable firmware file for A/B and rollback.

```sh
cd ~/niagara-drift
git status -sb
git fetch origin
git switch --track origin/experiment/ghost-event-audibility-mix50

sh tests/run_tests.sh &&
OM_FX_CPU1=1 ./build.sh &&
sh tests/run_tests.sh &&
cp build/omni.fwsc build/ghost-event-mix50-boost-EXPERIMENT.fwsc
```

Only flash after passing tests and build. The usual Windows Python
installer can flash from WSL:

```sh
WIN_INSTALLER=$(wslpath -w "$PWD/tools/fm1_install.py")
WIN_PACKAGE=$(wslpath -w "$PWD/build/ghost-event-mix50-boost-EXPERIMENT.fwsc")
python.exe "$WIN_INSTALLER" --info
python.exe "$WIN_INSTALLER" --backup
python.exe "$WIN_INSTALLER" "$WIN_PACKAGE"
python.exe "$WIN_INSTALLER" --info
```

On the FM-1, start with both parents, neither TAIL closed, GHOST MIX
25 then 50, and single-tap each key. BOW should now form a sustained
tonal coupling to the parents; SCRAPE should retain its new
parent-dependent roughness. Test double-tap latches and combinations
at modest MASTER volume first: the temporary gain boost can
increase peaks and FX drive.

With two latches running, check CPU1/CPU0 diagnostics before the
long STRESS V2 test. The tested half-rate branch had a 5804-us
deadline and a 5728-us recorded CPU1 peak. This is not a
verified timing result for the new firmware.
