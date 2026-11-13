---
title: LFO
intro: One clock, two waves, each with a shape, level and bypass
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

## LFO

Niagara Drift has two LFO waveforms (I and II) on a shared clock. Either waveform can be routed to either drone's pitch, X-Wave, cutoff or resonance.

## Controls

- **SELECT** sets the shared LFO rate (or cycle division when following MIDI clock).
- **ALGORITHM** controls Movement, which scales the LFO sends together from 0% to 200%.
- **Tap [[OCT-]]** or [[OCT+]] to advance LFO I or II to the next waveform. The Off entry is no longer part of the rotation.
- **Hold [[OCT-]]** or [[OCT+]] for at least 400 ms to bypass or restore LFO I or II, preserving the shape and level.
- **[[SEL]] + [[OCT-]]** toggles drone I in/out, and **[[SEL]] + [[OCT+]]** toggles drone II in/out. These act on the drone fade, not its filter cutoff.

The first [[LFO]] page lets you select either waveform directly using KNOB 1 or 3, and set the corresponding level using KNOB 2 or 4. The LFO's routing is on the next page.

## Waveforms

| LFO I | LFO II |
|---|---|
| Triangle | Tri 90 |
| Square | Square 90 |
| Saw | Exp saw |
| Roundtooth | Log tri |
| 2x Tri | 2x Saw |
| 2x Exp | 2x Pattern |
| 4x Pattern | 4x Exp |
| Random | Rnd glide |
| AND 3 | AND 5 |
| OR 3 | OR 5 |

## Good to know

- Bypassing an LFO retains its waveform and volume even when a scene is saved.
- The levels on the Waves page multiply the existing per-route depths; the four sends retain their balance.
- Movement scales the sends and delay-time modulation, not the separate Cross-mod circuit.
- At 0% Movement no LFO is heard on the routed destinations, regardless of the waveform's level.
