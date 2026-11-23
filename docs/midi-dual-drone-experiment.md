<!-- SPDX-License-Identifier: GPL-3.0-only -->
# External MIDI dual-drone trigger experiment

**Branch:** `experiment/volcano-midi-dual-trigger` (forked from the
merged `main` with half-rate GHOST and reactive excitation keys).

**Status:** Source and host regressions committed. This merged firmware
has **not** been compiled or hardware-timed in the assistant environment.

## Performance modes

**SEL+GLO** is the session-only performance-mode toggle. The MIDI mode
is not inferred from a cable insertion or the first Note On.

| | PLAY mode | MIDI mode |
|---|---|---|
| Enter | Power-up default; SEL+GLO from MIDI | SEL+GLO from PLAY |
| PLAY | Normal continuous drone transport | Arms/disarms MIDI-triggered audio without changing mode |
| At rest | Stopped until PLAY pressed | Both drone envelopes closed; LFO I/II phase clocks and Swell run |
| Drone tuning | Panel knobs, pitch link, sequences | External MIDI keys independently assign two drone pitches |
| Sequencer | SEL+PLAY available | Stopped; SEL+PLAY does not start it |
| Drone I/II outputs | SEL+OCT−/OCT+ | Gated only by external MIDI Note On/Off |
| GLO button without SEL | Setup pages | Setup pages |

Switching **into MIDI mode** starts both LFO transport clocks and forces
the Swell engine active without setting the patch's saved `P_PULSE`.
It closes both drone gates using global FADE and stops the sequencer
without deleting the authored note sequence. Releasing/pressing PLAY in
MIDI mode stops or restarts note triggering without switching modes.
Switching **out** with SEL+GLO fades to silence, returns to normal PLAY
mode in its stopped state, and preserves all saved pitches, PITCH LINK,
and the patch's Swell switch.

The physical FM-1 keys, scene controls, GHOST excitation keys and
other gestures are unchanged in either mode.

## Behavior

Only while MIDI mode is selected and PLAY is armed, incoming
**external USB MIDI or TRS MIDI** Note On/Off messages trigger
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
- When both drone slots are already sounding, the **third simultaneous
  held note** moves the existing drone **nearest in current sounding
  pitch** to its new target over the global **Scene Time** (or the
  current clock-aware Scene Bars duration when synced). The other drone
  keeps its pitch, and only two oscillator voices exist. If the
  original key on the chosen drone is released first, the glide
  continues while the third key remains held. If the third key is
  released first, the glide reverses toward its original held note.
  When neither remains held/sustained, that drone stops transitioning
  and fades out via global FADE. A fourth overlapping key is ignored.
- Ordinary Note On after a slot is freed STILL uses its former
  fade-out/retune/fade-in behavior, regardless of when the replacement
  arrives during the old note's FADE.
- Note On with velocity zero functions as Note Off. MIDI sustain
  pedal **CC64 >=64** latches notes after key release (including the
  third-note glide target); **CC64 <64** releases only physically
  released keys. Each channel's pedal state is independent. CC120
  and CC123 explicitly release notes on that channel, regardless
  of pedal state. Other MIDI messages, including MIDI clock,
  continue their original behavior.
- MIDI pitches are **temporary**, not written to saved project
  parameters. Only **SEL+GLO** exits MIDI mode. PLAY in MIDI mode
  stops or restarts MIDI note triggering; the stored pitch link
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

- `firmware/src/app/midi_duo.h`: two-slot allocator plus a single
  third-note pitch target; each note remembers physical key-down and
  per-channel CC64 sustain. A held original and third key may
  control the same existing drone, but there are never three voices.
- `firmware/src/app/ui.c`: SEL+GLO toggles mode and stops the
  sequencer on entry; Note On/Off/velocity-zero only reach the note
  allocator when MIDI mode is armed. Neither mode nor MIDI notes
  change `proj.par[GP_PITCH]`. PLAY remains a transport control.
- `firmware/src/dsp/drone.c`: explicit `C_MIDI_MODE` enables the
  two MIDI voice gates and temporarily forces the existing Swell
  engine active; `C_MIDI_NOTE` controls FADE-based attacks and
  releases. New `C_MIDI_GLIDE_TIME` + `C_MIDI_GLIDE` interpolate
  the *live* sounding pitch of the chosen existing drone over the
  Scene Time captured at key press, independently of global GLIDE.
  The transition continues if its old owner releases, reverses
  if the third key releases while the original is held, and freezes
  at the instantaneous pitch if both keys release. No new DSP voice
  or saved project parameter is allocated. The transport remains
  active for LFO, Swell and CPU1 GHOST/FX. On mode exit the global
  fade completes before the panel pitches are restored.
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

First test normal PLAY with incoming MIDI ignored, then SEL+GLO
entering a silent MIDI mode with LFO/Swell visibly running. Test
FADE short (~50 ms) and then about 1 s; release and replace each drone separately,
with held intervals, third notes, and ghost excitation active. Do a
new CPU0/CPU1 60-s timing check with GHOST MIX 50 and two MIDI
notes held before any prolonged STRESS V2 run. No new CPU1 waveform
or voice is introduced, but the merged firmware's real-time timing
has not yet been measured.
