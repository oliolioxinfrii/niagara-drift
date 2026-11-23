<!-- SPDX-License-Identifier: GPL-3.0-only -->
# External MIDI dual-drone trigger experiment

**Branch:** `experiment/volcano-midi-pedal-seq-curves`, based on the
hardware-tested sustained/chained MIDI glide experiment and its GHOST foundation.

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
| Sequencer | SEL+PLAY available; Sequence/Intervals/Transitions editable | Stopped; three SEQ edit pages dimmed/read-only, knob attempts show disabled banner |
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
- With two occupied drone slots, a third key glides the **nearest
  current sounding pitch** over global **Scene Time** (or clock-aware
  Scene Bars when synchronized). If both drones are equidistant, the
  drone with the **most recent accepted Note On** is chosen. The other
  drone stays at its pitch.
- Each drone independently supports one Scene-Time pitch transition.
  If a fourth key is closest to the **already gliding** drone, it is
  ignored without being redirected. If closest to the **other** drone,
  it starts another simultaneous glide. Neither case adds any voice.
- Releasing the original note during a glide does not interrupt
  its extra note. On **actual DSP completion**, a held destination
  becomes primary if the older original has been released.
  **With CC64 depressed**, the completed destination becomes primary
  even if the older note remains physically held or pedal-latched,
  allowing unlimited serial glides without touching the pedal. New
  notes targeting a drone still gliding are ignored (and require
  re-press), while the other drone can glide simultaneously.
- Releasing an extra key while its original remains held triggers a
  Scene-Time return glide; that voice remains unavailable to new
  overlapping glide targets until the return finishes. Releasing
  both stops its pitch change and fades out through global FADE.
- Ordinary Note On after a slot is freed STILL uses its former
  fade-out/retune/fade-in behavior, regardless of when the replacement
  arrives during the old note's FADE.
- Note On with velocity zero functions as Note Off. Sustain pedal
  **CC64 >=64** latches note releases and allows a finished glide
  destination to supersede the previous owner. **On CC64 <64**,
  a drone survives ONLY if its exact currently sounding note (or
  active glide destination) is physically held. If that destination
  has been lifted, the drone fades out using FADE, even if an
  earlier superseded key is still physically held. Other MIDI
  channels' sustain state is independent. CC120 and CC123 explicitly
  release notes regardless of pedal state.
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

## Sequencer curve update

The SEQ **Transitions** page retains its four independent curve graphs,
LATE–SINE–EARLY ruler, and knob layout, but the curve controls no longer
use continuous percentage sliders. Each detent steps through **Late,
Pw (cubic power), Quad, Sine, Line, Sqrt, Log, Early**. The graph
renders the exact interpolation used by the sequencer. Previously
saved percentages convert to the nearest named type when loaded.

While **SEL+GLO MIDI mode** is active, the Sequence, Intervals and
Transitions pages remain navigable but their existing content is dimmed
and read-only; turning a knob reports "SEQUENCER DISABLED" without
changing stored sequence data. The Partner and Pitch moves controls
remain available and SEL+PLAY cannot start the sequence.

## Implementation

- `firmware/src/app/midi_duo.h`: two-slot allocator with one extra
  target per drone, physical-note tracking, CC64 sustain and
  nearest-pitch/tie-breaking logic. Under pedal sustain, a completed
  target supersedes earlier held notes; pedal-up keeps only the
  physically held exact current note or glide target.
- `firmware/src/app/ui.c`: SEL+GLO toggles modes and stops the
  sequencer on entry. All SEQ note/interval/transition editors are
  disabled in MIDI mode; attempts show a banner. MIDI Note On/Off/CC64
  only change the transient allocator when MIDI mode is armed, never
  saved `proj.par[GP_PITCH]`.
- `firmware/src/dsp/drone.c`: explicit `C_MIDI_MODE` enables the
  two MIDI voice gates and temporarily forces the existing Swell
  engine active; `C_MIDI_NOTE` controls FADE-based attacks and
  releases. New `C_MIDI_GLIDE_TIME` + `C_MIDI_GLIDE` interpolate
  the *live* sounding pitch of the chosen existing drone over the
  Scene Time captured at key press, independently of global GLIDE.
  A per-drone completion epoch reports to the UI when that transition
  actually ends, ensuring serial glides are neither cut short nor
  prematurely unlocked before the DSP accepts the queued command.
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
git switch --track origin/experiment/volcano-midi-pedal-seq-curves
sh tests/run_tests.sh
OM_FX_CPU1=1 ./build.sh
sh tests/run_tests.sh
cp build/omni.fwsc build/volcano-midi-pedal-seq-curves-EXPERIMENT.fwsc
```

Keep the known-good firmware for rollback. The new named curve codes
(101..108) fit in existing scene sidecars and new firmware reads legacy
slider values, but an older firmware build will reject a sequence saved
with the new curve codes. Back up user sounds/settings before installing
or rolling back.

After all tests pass,
the usual Windows Python installer from WSL can flash it:

```sh
WIN_INSTALLER=$(wslpath -w "$PWD/tools/fm1_install.py")
WIN_PACKAGE=$(wslpath -w "$PWD/build/volcano-midi-pedal-seq-curves-EXPERIMENT.fwsc")
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
