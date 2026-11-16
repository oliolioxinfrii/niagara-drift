---
title: Playing in a browser
intro: The emulator: the mouse and the computer keys
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## Playing in a browser

The repository can build a browser emulator from its own source, allowing you to operate VOLCANO without an FM-1. The public Niagara Drift emulator is a different firmware and should not be used as a VOLCANO control reference.

## Mouse and keys

From this checkout, run `./emulator.command` or build with `sh web/emu/build.sh` and serve `build/emu`. See [Building](../../../BUILDING.md).

| Action | Mouse |
|---|---|
| Turn a knob | Drag vertically or scroll over it |
| Tap a button or piano key | Click it |
| Hold a button | Use the emulator's documented hold interaction |

The built-in emulator's keyboard assignments are specific to its version; consult its own interface or source for the current bindings. Do not assume that the public Niagara Drift emulator has the same controls.

## Good to know

- **SAVE** opens the same scene browser as the FM-1 firmware.
- **No pressure:** on keys 9–12 a held click climbs at the gesture's own time and falls when you let go. On the FM-1
  those keys follow how hard you press.
- **Working on the code:** the web build uses the checked-out firmware source; rebuild after source changes.
