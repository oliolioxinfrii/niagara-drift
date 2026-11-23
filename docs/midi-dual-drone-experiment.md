<!-- SPDX-License-Identifier: GPL-3.0-only -->
# External MIDI dual-drone trigger experiment

**Branch:** `experiment/volcano-midi-dual-trigger` (forked from the
merged `main` with half-rate GHOST and reactive excitation keys).

**Status:** Source and host regressions committed. This merged firmware
has **not** been compiled or hardware-timed in the assistant environment.

## Behavior

Incoming **external USB MIDI or TRS MIDI** Note On/Off messages trigger
the **two existing drones**. The physical FM-1 keyboard remains assigned
to the original STRIP/DARK/STILL/TAIL, pressure gestures and GHOST
STRIKE/SCRAPE/BOW/TREMOR; no local key is converted into a piano key.

- The first held external note takes **Drone I**, the second takes
  **Drone II**. MIDI channel does not determine the drone; channel
  and note together identify a held key for matching Note Off.
- When a note is released, its drone **fades out over global FADE**.
  The other drone, if held, remains playing.
- The next Note On takes **whichever drone was released**. It waits
  for the prior pitch to finish fading out before retuning and
  reopening that same drone. A new note is not glide-connected to
  the previous note; its oscillators snap to the new tuning while
  silent, then the global-FADE attack opens.
- If both drones still have held keys, a third Note On **waits
  silently**. When one assigned note is released, its slot is given
  to that waiting key, following that slot's FADE-out. Which slot
  changes is therefore determined by **which key was released**, not
  by the incoming note's order or MIDI channel. At most one waiting
  key is remembered (the most recent extra Note On has priority).
  Releasing an unassigned waiting key cancels it. This does not create
  a third voice or steal either held note.
- Note On with velocity zero functions as Note Off. Repeated Note On
  messages for the same held key are ignored. MIDI CC 120 or 123
  releases notes belonging to that channel. Other MIDI input,
  including MIDI clock, continues its original behavior.
- MIDI pitches are **temporary**, not written to saved project
  parameters. A physical PLAY press exits MIDI-trigger mode and
  returns to normal panel-drone transport. The stored pitch link
  setting remains unchanged but is bypassed while external MIDI
  controls independent pitches.
- Both the voice releases and attacks use **global FADE** (`P_FADE`),
  not MORPH TIME or a new MIDI envelope parameter. If PLAY was already
  sounding the original panel drones, their gates close using FADE
  before the first MIDI note retunes.
- GHOST, excitation latches, effects, tuning, filter settings and
  oscillators remain those of the two current drones. MIDI does
  not add polyphony, duplicate synthesizer voices, or write scenes.

### Example

| Event | Drone I | Drone II |
|---|---|---|
| Hold C2 | C2 | silent |
| While holding C2, hold C3 | C2 | C3 |
| Release C3, then press B3 | C2 | C3 fading out, then B3 |
| Keep B3 held; release C2, then press B2 | C2 fading out, then B2 | B3 |
| Release both | B2 fades out | B3 fades out |

With no held notes, a subsequent Note On again takes Drone I.

## Implementation

- `firmware/src/app/midi_duo.h`: two-slot held-note allocator; each
  note's MIDI channel is part of its identifier. No voice stealing.
- `firmware/src/app/ui.c`: receives Note On/Off/velocity-zero,
  routes only external MIDI into the allocator, and does not change
  `proj.par[GP_PITCH]`. PLAY releases MIDI ownership.
- `firmware/src/dsp/drone.c`: asynchronous `C_MIDI_NOTE` commands
  target the two existing voice gate envelopes. Once a gate has
  reached zero, queued replacement pitch is installed and its
  attack begins. Global PLAY fade is bypassed only while MIDI
  owns the voices; each voice still obeys `P_FADE` locally.
  MIDI can feed the existing CPU1 GHOST+FX pipeline while the
  global PLAY transport is stopped.
- `tests/host/midi_duo_test.c`: independent allocator tests.
- `tests/scenarios/midi-dual-trigger.omni`: full host MIDI routing,
  gate, fade-before-retune, saved-pitch independence, and PLAY handoff.

## Validation and build

Host tests and the combined build must pass before hardware flashing:

```sh
cd ~/niagara-drift
git status -sb
git fetch origin
git switch --track origin/experiment/volcano-midi-dual-trigger
sh tests/run_tests.sh
OM_FX_CPU1=1 ./build.sh
sh tests/run_tests.sh
cp build/omni.fwsc build/volcano-midi-dual-EXPERIMENT.fwsc
```

Keep the known-good firmware for rollback. After all tests pass,
the usual Windows Python installer from WSL can flash it:

```sh
WIN_INSTALLER=$(wslpath -w "$PWD/tools/fm1_install.py")
WIN_PACKAGE=$(wslpath -w "$PWD/build/volcano-midi-dual-EXPERIMENT.fwsc")
python.exe "$WIN_INSTALLER" --info
python.exe "$WIN_INSTALLER" --backup
python.exe "$WIN_INSTALLER" "$WIN_PACKAGE"
python.exe "$WIN_INSTALLER" --info
```

First test external MIDI while PLAY is off with FADE short (~50 ms),
then FADE of about 1 s; release and replace each drone separately,
with held intervals, third notes, and ghost excitation active. Do a
new CPU0/CPU1 60-s timing check with GHOST MIX 50 and two MIDI
notes held before any prolonged STRESS V2 run. No new CPU1 waveform
or voice is introduced, but the merged firmware's real-time timing
has not yet been measured.
