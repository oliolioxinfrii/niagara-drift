---
title: Controls
intro: The FM-1 buttons, page navigation and global controls
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Controls

VOLCANO uses the FM-1's four parameter encoders, four global encoders, page buttons and keys. You do not need to memorize the complete key map to begin: open a page and turn the four knobs shown beneath it.

> **Screenshot needed — FM-1 control map:** An annotated photograph or programmatically rendered panel generated from the v1.5 firmware's actual controls. Do not reuse the v1.3 `panel.png`.

## Global knobs

| Control | Function |
|---|---|
| **MASTER** | Overall volume |
| **SELECT** | Move backward or forward among pages in the current page family |
| **[[SEL]] + SELECT** | Set the common LFO rate (or MIDI-synced cycle division) |
| **PRESETS** | Gesture time: how quickly gestures develop |
| **ALGORITHM** | Movement: scale the LFO modulation depths |
| **KNOB 1–4** | Edit the four parameters shown on the current page; many drone pages use KNOB 1–2 for drone I and KNOB 3–4 for drone II |

## Page buttons

| Button | Pages |
|---|---|
| [[HOME]] | Drones, Pitch |
| [[ENV]] | Life, Tone |
| [[EDIT]] | Osc, Sub · Drive |
| [[LFO]] | Waves, Sends, Mods |
| [[SEQ]] | Partner, Pitch moves, Sequence, Intervals, Transitions |
| [[ARP]] | Swell, Swell shape |
| [[FX]] | Space, Details |
| [[GLO]] | Setup 1, Setup 2 |

Press a page button to open that family; pressing it again cycles through its pages. **HOME** starts at Drones and **FX** at Space. Other families can reopen the page you last used. Turning **SELECT** cycles pages within the active family; it does not switch between page families.

Most page buttons also support a temporary *peek*: hold the button, adjust a control, and release to return to the previous page. **ARP** is an exception: holding it for at least 400 ms switches Swell on or off.

## Other buttons

| Button | Function |
|---|---|
| [[PLAY]] | Fade the instrument in or out using the configured Fade time |
| [[SAVE]] | Open the scene browser to save, load, rename or transition between sounds |
| [[REC]] | Start recording a motion loop; press again to repeat it, and again to stop |
| [[SEL]] | Modifier for fine adjustments and secondary controls |
| [[OCT−]] / [[OCT+]] | Select or bypass LFO I / II waveforms |

**Hold HOME for 400 ms** to display the temporary VOLCANO activity atlas. It shows five geological layers representing effects, drone levels, Swell/sequencing, Feedback/Drift/Character and LFO activity. Releasing HOME returns to the previous page. See [Drones and Pitch](drones.md).

> **Screenshot needed — HOME atlas:** `build/scenarios/volcano-ui/geological-atlas.png`, captured from v1.5 source.

## Navigation and saved sounds

- **SAVE** opens the separate scene browser; white keys **1–8 are not scene shortcuts** during normal playing.
- **Keys 9–16** activate the eight gestures. Black keys control drone switches, linking and (on LFO pages) modulation routing.
- **On the Sequence page**, the keyboard enters notes rather than activating its ordinary switches or gestures.
- **Hold SEL and touch a black key** to display the key's purpose without changing it.
- **SEL + HOME** creates a randomized working sound and sequencer. **SEL + PLAY** starts or pauses the pitch progression without changing the instrument's fade.
- **SEL + SAVE** undoes or redoes the last supported scene/transition change.

The screen displays page names, current parameter values and action messages. VOLCANO uses five visual layer colors: cyan for FX, yellow for HOME, orange for SEQ/ARP, orange-red for EDIT/ENV and red for LFO. SAVE and GLO use separate system visuals.

For every button combination, see [Shortcuts](shortcuts.md). For the scene browser, see [Scenes](scenes.md).
