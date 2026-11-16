---
title: Help
intro: Installing, updating, connecting and recovering Volcano
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Quick answers

| Problem | What to check |
|---|---|
| A drone is silent | **SEL + OCT−** toggles drone I; **SEL + OCT+** toggles drone II |
| The pitch progression has stopped | **SEL + PLAY** starts, pauses or resumes the sequencer; normal PLAY controls the global audio fade |
| A preset is not at its programmed pitch | **Pitch Lock** ([[C#4]]) retains the existing pitch; switch it off to hear the preset's own pitch |
| Drone II wanders between notes | On SEQ → Pitch moves, reduce **Wander** (KNOB 3) |
| White keys 1–8 change the sound rather than loading scenes | They control STRIP, DARK, STILL and TAIL, separately for each drone. Use **SAVE** to browse scenes |
| The internal speaker has no sound, but USB or headphones work | Check **GLO → Setup 2 → Speaker** (KNOB 4) |

## Installing Volcano

Volcano is a **custom fork** of Niagara Drift. The original project's online installer and public emulator **do not** distribute this fork's firmware. This subtractive/STILL feature branch remains **unverified on physical FM-1 hardware**; do not treat it as a tested release package.

At present, build the desired source revision locally using [BUILDING.md](../../BUILDING.md). The installer package is `build/omni.fwsc`; install it with [Slipway](https://discodave-glitch.github.io/slipway/app/) or the repository's command-line installer:

```sh
python3 tools/fm1_install.py build/omni.fwsc
```

The firmware replaces M-VAVE's stock FM-1 software. Check the revision and any release notes before flashing development code. Installing firmware is at your own risk.

## Backups and recovery

Back up your scenes and settings before switching firmware revisions, and keep a stock-firmware recovery route available. The command-line installer supports `--backup` and `--restore FILE`; see [BUILDING.md](../../BUILDING.md) and the [installation utility](../../tools/fm1_install.py) for the current options.

If the FM-1 does not start normally, the repository describes its low-level restore path in [BUILDING.md](../../BUILDING.md#install). Holding **OCT− and OCT+** for five seconds enters the chip's own update mode, from which compatible recovery tools may restore the device.

For a failed update, disconnect other MIDI applications, reconnect the USB data cable directly and retry the verified installation procedure. For stock-firmware recovery, see [M-Flasher](https://github.com/jvitkauskas/m-flasher) and the instructions linked from BUILDING.md.

## Audio connections

- The **3.5 mm audio jack** provides audio for headphones or connection to another audio device with a suitable input.
- **USB-C** can carry the stereo audio signal to a compatible host. This is separate from using USB to install or send MIDI.
- **GLO → Setup 2 → Speaker** (KNOB 4) turns the built-in speaker on or off. Disabling it does not silence the 3.5 mm output or USB recording, and also bypasses the speaker-specific bass cut for the USB signal.

See [Setup](reference/setup.md#speaker-output) and [MIDI](reference/midi.md).

## Reporting problems

Use [this fork's issue tracker](https://github.com/oliolioxinfrii/niagara-drift/issues), not the upstream Niagara Drift tracker, for problems specific to Volcano. Include the branch/commit, what you did, what happened, and whether the behavior was reproduced in the host simulator, browser emulator or physical FM-1.
