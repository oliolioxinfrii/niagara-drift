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
