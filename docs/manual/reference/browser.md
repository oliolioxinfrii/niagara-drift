---
title: Playing in a browser
intro: The emulator: the mouse and the computer keys
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Playing in a browser

The repository can build a browser emulator from its own source, allowing you to operate VOLCANO without an FM-1. The public Niagara Drift emulator is a different firmware and should not be used as a VOLCANO control reference.

## Mouse and keys

From this checkout, run `./emulator.command` or build with `sh web/emu/build.sh` and serve `build/emu`. See [Building](../../../BUILDING.md).

| Do | Mouse | Keyboard |
|---|---|---|
| Turn a knob | drag up or down, or scroll | ↑ ↓ LFO rate, ← → Movement |
| Hold a button | Shift-click or right-click (click again to let go) | |
| PLAY · SAVE (hold) · SEL (hold) | | Space · Enter · C |
| HOME · EDIT · LFO I · LFO II (OCT− · OCT+) | | Esc · V · Z · X |
| Black keys 1–11 | click | 1 … 0, − |
| Scenes 1–8 · gestures 9–12 · gestures 13–16 | click | Q … I · A … F · G … K |

## Good to know

- **SAVE** opens the same scene browser as the FM-1 firmware.
- **No pressure:** on keys 9–12 a held click climbs at the gesture's own time and falls when you let go. On the FM-1
  those keys follow how hard you press.
- **Working on the code:** the web build uses the checked-out firmware source; rebuild after source changes.
