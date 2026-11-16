# Volcano

**A drone synthesizer for the M-VAVE FM-1.**

Volcano is a custom firmware based on [Niagara Drift](https://github.com/evanyerburgh/niagara-drift)
by Evan Yerburgh, replacing the FM-1's stock sound engine with two drones
that you can tune, shape, and set in motion.

The inspiration for these changes came about while I was using Niagara Drift,
which I want to give huge credit to—it's an amazing firmware. It immediately
made me think of the score for *Fire of Love* (2022) by Nicolas Godin.
It's an excellent documentary, which you should watch if you haven't!
In particular, I was thinking of the piece "Acid Lake," with its beautiful,
foreboding, and powerful drone sequence.

That desire to create a sequence in the drones led me to modify (and visually
change) the firmware into what it is now. The sequencer itself is much less
traditional than what you'd find in a grid-style sequencer; instead, it
allows up to four steps, each with individually controlled spacing and
progression.

This firmware, like Niagara Drift, is not intended to be an all-in-one
instrument; it is already demanding enough on the FM-1! As such, to create
the rest of the arrangements and elements from the soundtrack, I recommend
connecting the FM-1 to another instrument or recording setup. You can do
this with a 3.5 mm audio cable, or with USB-C for digital audio recording
through a compatible host. If using USB-C, turn the built-in speaker off
under **GLO → Setup 2 → Speaker**.

## Highlights

- **Two independently controlled drones.** Blend oscillator shapes, partner
  tones, subs and noise, then shape each drone with filtering and drive.
  Link their pitches or let the drones modulate one another.
- **Motion without constant playing.** LFOs, rhythmic Swell envelopes,
  Drift, Wander, and Age keep the sound evolving.
- **Performance controls.** Eight gestures transform several settings
  together; a motion loop can record and repeat manual changes.
- **Pitch sequencing.** Program up to four notes per drone, control their
  timing, and shape the transitions between notes.
- **Scenes and presets.** Start with 16 factory presets, save up to 48
  named scenes, and transition gradually between sounds using the scene
  browser.
- **Effects.** Stereo delay, reverb, drive, and feedback extend the drones
  from subtle movement to sustained textures.
- **Volcano interface.** Geological, waveform, and orbital displays make
  modulation, sound shaping, scene selection, and stereo settings visible.

## Getting started

The FM-1's buttons open groups of sound controls. Turn **SELECT** to move
between pages in the current group; turn the four encoders above the
buttons to change the displayed parameters. **SAVE** opens the scene
browser. Hold **HOME** to view the Volcano activity atlas.

For a complete explanation of the controls, see the
[manual](docs/manual/index.md). Its
[reference](docs/manual/reference/index.md) covers the individual pages,
while [recipes](docs/manual/recipes/index.md) demonstrate ways to build
sounds. Some legacy manual pages still describe controls that changed in
v1.4/v1.5; consult [v1.4 controls](docs/v1.4-controls.md) and the
[Volcano v1.5 specification](docs/volcano-v1.5-ui.md) for those changes
until the manual is brought up to date.

## Installing

**Volcano does not currently have a published firmware download.**
Build the desired revision locally following [BUILDING.md](BUILDING.md).
The resulting `build/omni.fwsc` package can be installed using
[Slipway](https://discodave-glitch.github.io/slipway/app/) or the bundled
command-line installer.

Installing custom firmware replaces the FM-1's stock firmware. Before
installing, review the [backup, recovery, and troubleshooting
instructions](docs/manual/help.md). The
[upstream online emulator](https://evanyerburgh.github.io/niagara-drift/)
runs Niagara Drift rather than Volcano; instructions for building
the local emulator are in [BUILDING.md](BUILDING.md).

## Documentation and development

- [Manual](docs/manual/index.md) — recipes, controls, shortcuts, and help
- [Building and testing](BUILDING.md) — toolchain, simulator, emulator, and firmware
- [Changelog](CHANGELOG.md) — release and development history
- [Issues](https://github.com/oliolioxinfrii/niagara-drift/issues) — problems specific to Volcano

## Credits and license

Volcano is based on Niagara Drift by Evan Yerburgh. The underlying
firmware draws on work from
[Felucca](https://github.com/hugelton/Felucca),
[X0X](https://github.com/charlesvestal/fm1-x0x),
[FoMni](https://github.com/charlesvestal/fm1-fomni),
[MONK-1](https://github.com/charlesvestal/fm1-monk),
[ChoralRoot](https://github.com/Quixotic7/ChoralRootFM1),
[fm1-base](https://github.com/charlesvestal/fm1-base), and the
[FM-1 Screen Reader](https://github.com/charlesvestal/fm1-reader).
The original inspiration includes the Stylophone CPM DS-2,
Moog Subharmonicon, and Soma Lyra-8.

Licensed under GPL-3.0-only, except for the MPL-2.0 components identified
in [LICENSING.md](LICENSING.md). Volcano is not affiliated with or
endorsed by M-VAVE or the makers of the instruments that inspired it.
