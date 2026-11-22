<!-- SPDX-License-Identifier: GPL-3.0-only -->
# GHOST half-rate Cross-Synth experiment (22.05 kHz)

**Experimental branch:** `experiment/ghost-cross-half-rate-22050`  
**Reference:** `feature/ghost-voice-candidate` at `3163003`  
**Status:** source-only experiment; host suite, FM-1 hardware timing and listening **not yet tested**.

## Objective

Relieve CPU1's nearly saturated GHOST+FX workload without moving Cross-Synth to
CPU0 or altering the two parent drones, Wave Terrain, shared eight-mode body,
GHOST compressor, or the FX pipeline. GHOST MIX still controls the **output**
crossfade and the hybrid timbre remains fixed at the original 50% internal mix.

The reference Niagara hardware results with GHOST MIX 50 and both drones
playing were:

- CPU0 average load 51.0%, recorded longest render 3370 us; zero new late
  blocks in the 60-second interval.
- CPU1 recent smoothed mean 5290 us, completed-job peak 5644 us, zero missed
  handoffs and zero faults; the audio deadline is about 5804 us.
- A subsequent MIX-100 test reported mean 5312 us and cumulative peak
  5678 us. MIX 0 (after the fade completed) bypassed GHOST, with CPU1
  recent mean 1865 us. The difference is *not* a separately timed stage.

## Change under test

**Only** Cross-Synth's two twelve-band TPT SVF banks execute at half rate,
22,050 Hz. The remaining GHOST and shared FX operations stay at 44,100 Hz.

- Full-rate parent samples pass through two 15-tap Hamming-window
  halfband FIR anti-alias filters (one per parent), then decimate by two.
- All twelve original filter center frequencies (35-2405 Hz) and the
  damping parameter are preserved; TPT coefficients are calculated for
  22.05 kHz. The highest center is far below the new 11.025-kHz Nyquist.
- Envelope and post-filter low-pass/DC tracking coefficients are transformed
  with `alpha_half = 1 - (1 - alpha_full)^2` to preserve their approximate
  time constants.
- The decimated Cross-Synth output is linearly interpolated back to
  44.1 kHz before mixing with the full-rate body output. The interpolation
  is causal. Input FIR group delay is seven full-rate samples, and
  reconstruction adds approximately another one-sample delay relative to
  the prior full-rate Cross-Synth path. Differences in transient phase,
  high-harmonic content and timbre are possible.
- Reset clears the decimator history and reconstruction state. No new CPU0
  work, mailbox fields or runtime controls are introduced.
- Compile-time `OM_GH5_HALF_RATE` defaults to 1 on this experiment branch.
  Defining it as 0 restores the original full-rate SVF, envelope and
  post-filter equations. The unmodified reference is also available
  directly on `feature/ghost-voice-candidate`.

This is an attempt to save CPU1 time, **not** a validated speedup or a proof
of transparent resampling. FIR attenuation near the transition band is
finite; listening should include noisy/bright sources, transitions and
high harmonics. The 15-tap input filter is not a brick-wall anti-alias
filter.

## WSL build

From a clean checkout of this branch in `~/niagara-drift`:

```sh
cd ~/niagara-drift
git status -sb
git fetch origin
git switch --track origin/experiment/ghost-cross-half-rate-22050
git rev-parse --short HEAD
source .venv/bin/activate
sh tests/run_tests.sh
OM_FX_CPU1=1 ./build.sh
sh tests/run_tests.sh
cp build/omni.fwsc build/ghost-cross-half-rate-EXPERIMENT.fwsc
```

If the branch is already present locally, use `git switch
experiment/ghost-cross-half-rate-22050` and `git pull --ff-only`.
Protect any uncommitted work before switching.

## Hardware comparison

Retain the previously tested full-rate `.fwsc` and a saved-scene backup
before flashing. To install over USB-MIDI from the home WSL Bash terminal,
call **Windows Python** (Linux ALSA sequencer is absent on this host):

```sh
cd ~/niagara-drift
WIN_INSTALLER=$(wslpath -w "$PWD/tools/fm1_install.py")
WIN_PACKAGE=$(wslpath -w "$PWD/build/ghost-cross-half-rate-EXPERIMENT.fwsc")
WIN_CPU0=$(wslpath -w "$PWD/tools/fm1_cpu.py")
WIN_CPU1=$(wslpath -w "$PWD/tools/fm1_fx_cpu.py")
python.exe "$WIN_INSTALLER" --info
python.exe "$WIN_INSTALLER" --backup
python.exe "$WIN_INSTALLER" "$WIN_PACKAGE"
python.exe "$WIN_INSTALLER" --info
```

After a full power cycle, use **Niagara playing, GHOST MIX 50**, with
the same drone and FX controls as the 5290-us/5644-us baseline. Record:

```sh
python.exe "$WIN_CPU1" 60
python.exe "$WIN_CPU0" 60
```

The reported CPU1 mean is a *recent smoothed value*, not the entire
interval average. Its peak is cumulative since binding. Record missed
handoffs/faults and CPU0 late-block **deltas**. Power-cycle before a clean
peak comparison. Listen to bright/noisy parent settings, low pitches,
GHOST TAIL release, and FX tails. If Niagara remains playable with useful
headroom, try a longer run; **STRESS V2 is a later qualification test**.
CPU1 retirement means the experiment did not meet the deadline under
that workload.

## Interpretation

A successful experiment requires a **meaningful measured reduction in CPU1
time**, no missed handoffs during representative playing, and an acceptable
GHOST timbre. A sonic change is permissible for this secondary voice, but
the unchanged Mode-14 branch remains the more thoroughly tested fallback.
