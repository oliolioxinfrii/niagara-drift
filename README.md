# Niagara Drift

**A drone synth for the M-VAVE FM-1.**

> **Custom Niagara Drift fork (v1.3).** The `main` branch includes the tested
> two-drone pitch sequencer alongside the expanded Swell, three-page LFO, keyboard
> gestures, and UI improvements. This is **not** an official Niagara Drift release;
> the original project is [evanyerburgh/niagara-drift](https://github.com/evanyerburgh/niagara-drift).

> **Experimental v1.4 branch:** This branch changes SELECT navigation, the
> SAVE browser, per-segment sequencer curves, and per-slot scene naming. Keys 1–8 are
> reserved (unassigned) outside SEQ note entry; the controls illustrated
> below describe the stable **v1.3** layout. For the v1.4 control map,
> including **SAVE = yes**, **HOME = back**, and **REC = rename** inside
> the SAVE browser, see [v1.4 controls](docs/v1.4-controls.md).
> **Not yet verified on hardware. Do not install until the host and web
> emulator regression tests pass.**

Niagara Drift is open-source firmware that turns the M-VAVE
FM-1 into a drone synthesizer: two always-on drones you tune, shape and move rather than play note by
note.

![The FM-1 with what every control does in Niagara Drift written on it](docs/img/panel.png)

*Your FM-1 with Niagara Drift installed: what each knob, button and key now does. Amber is drone I,
teal drone II, lilac the LFO. The black keys are switches (lit = on; on the LFO pages they set the
LFO's routing, and cross-mod lets drone I move drone II's filter, or the drones and their sound bend each other's pitch); white keys 1–8 hold scenes, in
eight banks you pick with SEL and the gesture keys; keys 9–12 and the top four are gestures. The page buttons' pages, each tap the next: HOME
Drones, Pitch · ENV Life, Tone · EDIT Osc, Sub · Drive · SEQ Partner, Pitch moves,
Sequence, Intervals · LFO Waves, Sends, Mods · ARP Swell, Swell shape · FX Space, Details ·
GLO Setup 1, 2, Speaker.*

**Status:** The v1.3 release-candidate firmware (`FM-1_8010300`) built successfully with the
JieLi toolchain, passed the complete WSL host, OTA, and WebAssembly test suite, and was tested
on a physical FM-1 in November 2026. Slight lag has been observed when Sequence, Swell, and
LFO all run at very short timescales; its cause has not yet been profiled. The
[creator's online emulator](https://evanyerburgh.github.io/niagara-drift/) runs upstream code,
**not this fork**. For this exact source use the local web emulator
(`sh web/emu/build.sh`, then `python3 -m http.server 8765 --directory build/emu`).

## What it does

- **16 Presets** to start from, each a different side of the instrument and each moving over a minute: Niagara,
  Embers, Vespers, Dreamhouse, Abyss, Seance, Sonar and Sibyl on keys 1–8 of the Presets bank; Howl, Cistern, Piston,
  Ashram, Telegraph, Tape, Falling and Bare on keys 1–8 of the second.
- **Eight gestures**, each several settings moving together on top of your sound: **Weight**, **Shift**, **Clean** and
  **Wash** on the top keys, in while you hold and out as slowly as you held; **Tick**, **Chord**, **Snarl** and
  **Stutter** on keys 9–12, as deep as you press. Tap any of them in to stay.
- **Scenes**: 8 on the white keys in each of six banks (48 of yours), plus the Presets' two, recalled gliding over the
  Scene time, or **crossfaded into by pressing the key harder**; a **motion loop** that records your knob moves, gestures and scenes.
- **It moves on its own**: the LFO, the Swell, **Drift**, **Wander** (drone II's slow random melody on a scale) and
  **Age** (tape wear) keep it breathing without a hand on it.
- **Two drones**, each a main oscillator that blends sine → triangle → saw → square (X-WAVE), a
  partner oscillator at an interval (tempered or just) with detune, a sub one to four octaves down
  (the lowest two click under a low drone) or a subharmonic (⅓, ⅕ of the pitch), and a white or pink noise, into its own self-oscillating
  filter (low-, band- or high-pass) with drive. Pitch is free between notes, from 2 Hz to 1.6 kHz.
- **Links** between them: pitch link, hard sync, and cross-mod: drone I's sound moving drone II's cutoff, or each
  drone's sound bending the other's pitch (cross-FM, as on the Lyra-8).
- **The Ultra LFO**, after the DS-2's: one rate, two waves of ten shapes each (one of them truly
  random, two the Lyra-8's stepped AND and OR), routed to each drone's pitch or X-WAVE and cutoff or resonance, alone or summed. It can
  move its own rate and lock to the Swell.
- **Movement** (ALGORITHM): all the LFO's depths down or up together, from still to double.
- **The Swell**: both drones breathing in time, from a slow soft swell to a hard rhythmic gate, with swing.
- **Sequence**: a looping, keyboard-authored progression of 1–4 pitches per drone, with independent
  event positions, linked-pitch behavior, a phase offset for drone II, and a proportional timing
  display. Its pitch motion follows the programmed tuning; the loop can last up to four minutes.
- **Effects**: a stereo delay of six types (Clean, Wide, Ping-pong, Tape, and the Lyra-8-like Grain and Self, whose
  own echoes move their time; the LFO can warble it too), a drive
  after it (the drones and their echoes distorted together, as on the Lyra-8), into a reverb of five (Room, Hall,
  Cathedral, Chorus hall, Shimmer) whose length goes to Inf, a freeze.
- **Feedback** (the finished sound fed back into the drones) and **Character** (clean to vintage).
- **Drift**: the drones are never quite still; pitch, cutoff, timbre, level and detune slowly wander.
- **Morph**: buttons and keys never jump: the sound moves to a new setting over a time you set.


### Sequencing in v1.3

SEQ cycles through **Partner**, **Pitch moves**, **Sequence**, and **Intervals**:

- **Sequence (page 3):** KNOB 1 selects the step; SEL + KNOB 1 retunes it on the selected
  tuning grid; KNOB 2 selects drone I or II; KNOB 3 chooses 1–4 steps; KNOB 4 offsets drone
  II's progression phase. Press a keyboard key on this page to enter and audition a note.
  The animated amber and teal curves show the pitch paths; step 1 follows HOME → Pitch.
- **Intervals (page 4):** KNOBs 1–3 move the arrival of steps 2–4, where those steps exist.
  KNOB 4 scales the total loop length without changing the relative positions of the events.
  Both drone timelines fill the display; drone II's markers reflect its phase offset.
- **Transport:** SEL + PLAY starts, pauses, or resumes the pitch progression at its current
  position, without changing the global audio level. Ordinary PLAY controls the instrument's
  fade; the sequencer resets its pitch only after the global fade-out reaches silence.
- **Random Start:** SEL + HOME randomizes the sound and a new 2–4-step sequence, including
  the note pitches, step spacing, overall duration (20–240 seconds), and drone II phase.
  Consecutive notes, including the loop return, differ by no more than one octave. Pitch
  Lock and Pitch Link keep drone II coupled to drone I.

The sequencer is kept in RAM, independently of scenes and the Motion Loop. Its notes and
timing are not saved with a scene. The Speaker setting and improved waveform/ARP LED
brightness from the earlier Swell/UI work are also included.

## Installing this custom build

This fork's installer package is built locally; the original creator's website and GitHub releases
serve **their** firmware, not this fork. To build the exact source on `main` or a feature branch,
see [BUILDING.md](BUILDING.md):

```sh
./build.sh
tests/run_tests.sh
```

The installable file is `build/omni.fwsc`; a versioned release-candidate build produces
`build/niagara-drift-1.3-rc1.fwsc`. Keep a previously working package and a
stock-firmware recovery route. The v1.3 candidate can be built and tested together using
`sh tools/sequence_release_candidate.sh` (with the JieLi toolchain, AC79 SDK, and Emscripten
available). To install on a physical FM-1, use
[Slipway](https://discodave-glitch.github.io/slipway/app/) to select the local `.fwsc`
package, or use the bundled CLI (`pip install mido python-rtmidi`):

```sh
python3 tools/fm1_install.py build/omni.fwsc
```

No custom firmware binary has been published as a release from this fork.

**What it replaces:** Niagara Drift replaces M-VAVE's original sound engine and sequencer.
The custom firmware includes its own two-drone pitch sequencer; it is not the stock FM-1
sequencer.

**Updating:** when there's a new version, run the installer again the same way. Your sound and scenes are
kept, unless that version's notes in the [changelog](CHANGELOG.md) say otherwise.

**Backing up, going back to M-VAVE's firmware, and when an install doesn't take:** see
[Help](docs/manual/help.md). Installing firmware is at your own risk.

## Manual

- [Start here](docs/manual/index.md): the manual for the source in this fork
- [Recipes](docs/manual/recipes/index.md): how to get a sound or a move, across the pages
- [Reference](docs/manual/reference/index.md): every page, knob and key
- [Shortcuts](docs/manual/reference/shortcuts.md): every key combination, hold and tap
- [Help](docs/manual/help.md): installing, backing up, going back, problems

The [creator's site](https://evanyerburgh.github.io/niagara-drift/) provides the upstream
emulator and manual, which may differ from this custom firmware. Build this repository's web
emulator to inspect its current UI.

## Problems and ideas

For problems specific to this custom firmware, use the
[issues in this fork](https://github.com/oliolioxinfrii/niagara-drift/issues).
For general upstream issues, consult the [original repository](https://github.com/evanyerburgh/niagara-drift).
Include the commit or branch, the firmware identity, and the steps to reproduce.

## Building and testing

See [BUILDING.md](BUILDING.md) for the toolchain, including the extra steps on a Mac.

```
tests/run_tests.sh      # the instrument, the platform pieces, the app in a simulator
./emulator.command      # the same code in the browser: builds it, serves it, opens it
./build.sh              # build/omni.fwsc, the installable firmware
```

`build/host/omni_host SCRIPT OUTDIR` runs the whole app on a computer from a script
(`tests/scenarios/*.omni`), writing audio and screenshots. `python3 tools/make_screens.py` remakes the pictures of the pages
above (`docs/img/page-*.png`) from `tests/scenarios/pages.omni`. The manual's pages are Markdown in `docs/manual`;
`python3 tools/make_manual.py OUT` makes them into the site's pages (the emulator's build does), and
`python3 tools/make_manual.py --renders` remakes each recipe's screens and sound from its script in
`docs/manual/renders`.

Installing firmware is at your own risk. Before installing a development build, make sure you can
put M-VAVE's firmware back (see BUILDING.md).

## Credits

Niagara Drift is the next link in a chain of FM-1 firmwares:

- [Felucca](https://github.com/hugelton/Felucca) by Leo Kuroshita (Hügelton Instruments): the
  platform (hardware layer, USB, update loader, storage, installer), and a reference for the
  sound engine.
- [X0X](https://github.com/charlesvestal/fm1-x0x) and [FoMni](https://github.com/charlesvestal/fm1-fomni)
  by Charles Vestal: the platform reshaped for a single instrument, the host simulator and the
  browser emulator. Niagara Drift started from FoMni's code.
- [MONK-1](https://github.com/charlesvestal/fm1-monk) by Charles Vestal: reading key pressure (from
  [Velo](https://github.com/charlesvestal/fm1-velocity)'s sensing), which plays the scene crossfade.
- [ChoralRoot](https://github.com/Quixotic7/ChoralRootFM1) by Quixotic7: the design of the delay and the
  reverb, written anew here; the idea of crossfading between scenes, and of a backup before installing.
- [fm1-base](https://github.com/charlesvestal/fm1-base) by Charles Vestal: the safe restore's flash core (staging,
  the journal, the commit and its finish at power-on), as it is there (MPL-2.0).
- The [FM-1 Screen Reader](https://github.com/charlesvestal/fm1-reader) by Charles Vestal: the screen's words over
  USB and the page that speaks them (its firmware side from fm1-base, MPL-2.0); thanks to João Durães (WoodJack) for
  asking for it.

It began from ideas in the Stylophone CPM DS-2, the Moog Subharmonicon and the Soma Lyra-8.

GPL-3.0-only (the files from fm1-base keep their MPL-2.0); see [LICENSING.md](LICENSING.md).

Niagara Drift isn't affiliated with or endorsed by M-VAVE, Hügelton Instruments or the makers of any
instrument that inspired it.
