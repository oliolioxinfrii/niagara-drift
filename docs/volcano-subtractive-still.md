<!-- SPDX-License-Identifier: GPL-3.0-only -->
# Volcano: subtractive gestures and independent STILL clocks

Status: **draft feature branch**, not validated on an FM-1. The v1.5 UI merge is the base; this change does not alter the main branch.

## White-key map

| Key | Drone I, keys 1–4 | Key | Drone II, keys 5–8 |
|---|---|---|---|
| 1 | STRIP | 5 | STRIP |
| 2 | DARK | 6 | DARK |
| 3 | STILL | 7 | STILL |
| 4 | TAIL | 8 | TAIL |

The remaining eight white keys keep the established gestures, in physical order: **9 Tick, 10 Chord, 11 Snarl, 12 Stutter, 13 Weight, 14 Shift, 15 Clean, 16 Wash**. On the sequencer's note-entry view, the lower keys remain note-entry keys rather than gestures. The legacy scene-pad test mode also retains its scene-key mapping.

* **STRIP** reduces the partner, sub and noise contributions, leaving the main oscillator at full depth. The mixer crossfade shifts toward the main oscillator so the sub contribution is reduced.
* **DARK** moves the effective X-WAVE output toward sine and reduces drive and Character harmonic generation. It does not add another filter.
* **STILL** slows the LFO, Swell and sequencer *progression* for one drone, from normal speed at 0% to zero speed at 100%. Oscillator audio, effects, Drift, Wander and Age are not frozen.
* **TAIL** scales the selected drone's contribution into the common dry/effect bus toward silence. Existing shared delay and reverb buffers continue ringing; tails cannot be separated by source after entering shared effects.

These four depth values per drone are **session/performance state** in this first draft; unlike the existing eight gesture latches, they are not yet part of saved scenes.

## Common key action

All 16 gesture keys are independent of pressure/velocity. Holding a key increases the corresponding amount linearly, from its current value, at **100% per Morph Time** (the PRESETS encoder currently also adjusts Morph Time). The amount stops at 100%. Releasing a held key retains its exact amount; no second latch action or SEL modifier is required.

A short tap of an already active, stationary key begins a linear return to zero; tapping during a return pauses it; tapping once more resumes the return. A press lasting longer than 220 ms resumes increasing from its present value. A short press from zero retains its new, possibly small, amount. Pressed keys use a short tap-recognition delay when they are already active to distinguish a new hold from a tap. The on-screen header reports a percentage at every stage, without a separate “latched” label.

The existing eight gestures retain saved latches in `P_GL_0...P_GL_7`. Their motion-loop control events remain available. The PRESETS encoder's former separate Gesture Time control is replaced by access to the shared Morph Time setting in this draft; the existing `proj.mtime` field remains for backwards compatibility.

## Sequence display, scene persistence and SEL

- The **Intervals** page's two running playheads use each drone's own STILL-adjusted
  sequencer clock, matching the **Sequence** page. The score's programmed event
  markers remain at their authored positions.
- **Saving a user scene (A–F)** stores its current sequence with the sound: 1–4
  steps, two drones' note pitches, event intervals, interpolation shapes, selected
  drone and phase offset. Recalling the slot restores its sequence, stopped and
  reset to step 1. The sound's first pitch remains authoritative for step 1.
  Transport state, editor cursor and STILL offsets are not stored. An older
  scene without sequence data recalls as one step; factory Presets remain
  one-step sounds.
- Sequence data is kept in versioned trailers on the existing bank flash
  objects, including A's sidecar on the historical overwritten-Presets object.
  It does not change the existing sound/project binary format or discard
  unmigrated Preset data.
- **Holding SEL no longer replaces the active page** with a key-map popup.
  SEL's fine-tuning and control-modifier shortcuts remain available.

## SAVE browser color identity

The SAVE browser cycles **P1 → P2 → A → B → C → D → E → F**, progressing
through the FX parameter-value ramp from deep blue (`#174C76` on P1) to
near-white blue (`#EFFCFF` on F). **P2 is the second-darkest shade**, not
the near-white endpoint. Every node retains the same deep-blue center
regardless of bank; outline brightness distinguishes empty and occupied
slots, while ticks identify the selected slot. The satellite retains its
original geological colors, but is smaller and sits between the two
lower-left orbit nodes without covering either node.

## SAVE browser: Load versus Transition

**LOAD** is an editing and startup command. Confirming a populated slot applies
its stored sound immediately rather than waiting for the Scene Time. The saved
sequence is restored, stopped at step 1. The global room settings (including
Pitch Lock, Scene Time, speaker, and external clock settings) are not replaced,
but the saved pitches are applied even while Pitch Lock is active. Any in-flight
scene morph is canceled. The menu shows **TIME: NOW** for this action.

**TRANSITION** is the performance command. It moves from the current sound
toward the chosen target over **Scene Time** (or the configured clock-synced
bars), ending at the selected **MORPH** percentage. For example, 40% ends
40% of the way from the current sound to the stored sound. Existing Pitch Lock,
room exclusions and discrete-control threshold behavior remain in force.
Partial transitions leave the current sequence in place; a 100% transition
loads the target's authored sequence **only after Scene Time completes**, so
the pitch progression cannot jump at the start of a performance transition.

The Sequence → Intervals page change explicitly invalidates all three LCD
strip caches, preventing the first Intervals screen from retaining the prior
Sequence page's trace.

## LFO and Swell transport (C#5)

A short C#5 tap pauses both LFO waves and their per-drone phase markers at the
current position; another tap resumes from the same phase. The Swell is not
paused by a short LFO tap. A hold of 500 ms enters a **smooth stop**: the LFO
master and both drones' LFO and Swell clocks rewind from their current
positions to phase zero at the *LFO's current rate*. At zero, both LFO and
Swell clocks immediately resume forward motion, like a rubber-band turnaround:
no additional tap, pause, or jump at the endpoint. The return time therefore
depends on LFO rate; it is not an instantaneous reset or a fixed-duration
animation. If Swell phase offset is configured, that musical offset is
preserved after the turnaround. External clock steering
is suspended while paused/stopping to prevent a phase discontinuity.

The previous C#5 **LFO-to-Swell synchronization lock** is retired as a
performance control. Its legacy project parameter retains its place in the
binary preset format for compatibility, but no longer restarts the LFO on
Swell boundaries. The earlier direct DSP reset command is retained for
host tests; it is not the long-press behavior.

## STILL clock model

The common master clocks continue running. For each drone and each of the three systems (LFO, Swell, sequencer), the effective time is a *local clock* derived from the master plus an independently accumulated phase error. That means both drones may freeze simultaneously without referencing each other. Their programmed LFO rate, Swell rate, sequence timing, and phase settings remain unchanged.

For a STILL depth `s` between zero and one, local speed is `(1-s)` times master speed while the key is held or stationary at a nonzero depth. Return to zero initiates cubic Hermite correction of the local-to-master phase error. Both the error and its time derivative reach zero by the time the gesture returns to zero. The elapsed correction duration is therefore proportional to the remaining STILL depth and uses Morph Time.

Cubic phase catch-up may temporarily move *faster* than the master to make up a phase deficit; this draft does **not** implement an independent maximum catch-up speed. When a user pauses a return, the phase correction pauses as a new stationary offset. Master LFO resets preserve the audible local phase while STILL is active. Random LFO shapes require independent per-drone random history, implemented without audio buffers.

The LFO wave/modulation screens, Swell graph and sequencer score show separate colored drone progress markers. They use the local clocks that control audio, not independent animation timers.

## Verification before merging

- Run `CC="cc -D_DEFAULT_SOURCE -Wl,--no-as-needed -lm" sh tests/run_tests.sh`, including the new `still_test`. Fix all regressions in legacy scenarios and render golden files.
- Build the web emulator and compare the waveform displays against the sounds at 0%, 50% and 100% STILL; test both drones frozen simultaneously and independently returning.
- Check rapid taps, repeated holds, pauses mid-return, 100% saturation and a very short Morph Time; verify program/scenes and Motion Loop behavior.
- Check MIDI-clock sync and the new C#5 pause/resume and smooth LFO/Swell rewind while STILL holds and rejoins; examine random LFO waveforms and sharp-step sequencing.
- Record dry/wet WAVs and measure spectral changes for STRIP/DARK and effect tails for TAIL, including cross-FM and feedback.
- Run `./build.sh`, `tools/budget.py` and hardware `tools/fm1_cpu.py`; ensure flash, .data+.bss, pool headroom and interrupt CPU budget are within FM-1 limits.
- Check physical key debounce, latency, visual legibility and frame rate on the M-VAVE FM-1 before a merge.
- Run `tests/scenarios/save-menu-instant-load.omni` to distinguish immediate LOAD
  from a timed 40% TRANSITION and verify both scenes retain separate sequences.
- Run `tests/scenarios/seq-intervals-first-entry.omni`: the first and repeat
  Intervals screenshots must be byte-identical. `tests/run_tests.sh` compares them.
- Rebuild after sequence-storage changes; test saving distinct sequences in A–F
  and recalling after reboot, including through the SAVE browser. Confirm
  existing scenes and older overwritten Presets remain intact.

No firmware build, host suite, or hardware measurement has been completed by this change alone.
