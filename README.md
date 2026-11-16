# Niagara Drift

**A drone synthesizer for the M-VAVE FM-1.**

Niagara Drift replaces the FM-1's stock sound engine with two continuously
sounding drones that can be tuned, shaped, and set in motion. This custom
fork includes the **VOLCANO v1.5** interface, with animated visualizations
of the instrument's sound and controls.

Niagara Drift was created by [Evan Yerburgh](https://github.com/evanyerburgh/niagara-drift).
This repository is a custom fork, not an official upstream release.

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
- **VOLCANO interface.** Geological, waveform, and orbital displays make
  modulation, sound shaping, scene selection, and stereo settings visible.

## Getting started

The FM-1's buttons open groups of sound controls. Turn **SELECT** to move
between pages in the current group; turn the four encoders above the
buttons to change the displayed parameters. **SAVE** opens the scene
browser. Hold **HOME** to view the VOLCANO activity atlas.

For a complete explanation of the controls, see the
[manual](docs/manual/index.md). Its
[reference](docs/manual/reference/index.md) covers the individual pages,
while [recipes](docs/manual/recipes/index.md) demonstrate ways to build
sounds. Some legacy manual pages still describe controls that changed in
v1.4/v1.5; consult [v1.4 controls](docs/v1.4-controls.md) and the
[VOLCANO v1.5 specification](docs/volcano-v1.5-ui.md) for those changes
until the manual is brought up to date.

## Installing

**This fork does not currently provide a published firmware download.**
Build the desired revision locally following [BUILDING.md](BUILDING.md).
The resulting `build/omni.fwsc` package can be installed using
[Slipway](https://discodave-glitch.github.io/slipway/app/) or the bundled
command-line installer.

Installing custom firmware replaces the FM-1's stock firmware. Before
installing, review the [backup, recovery, and troubleshooting
instructions](docs/manual/help.md). The
[upstream online emulator](https://evanyerburgh.github.io/niagara-drift/)
runs the original project rather than this fork; instructions for building
the local emulator are in [BUILDING.md](BUILDING.md).

## Documentation and development

- [Manual](docs/manual/index.md) — recipes, controls, shortcuts, and help
- [Building and testing](BUILDING.md) — toolchain, simulator, emulator, and firmware
- [Changelog](CHANGELOG.md) — release and development history
- [Issues](https://github.com/oliolioxinfrii/niagara-drift/issues) — problems specific to this fork

## Credits and license

Niagara Drift builds on work from
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
in [LICENSING.md](LICENSING.md). Niagara Drift is not affiliated with or
endorsed by M-VAVE or the makers of the instruments that inspired it.
