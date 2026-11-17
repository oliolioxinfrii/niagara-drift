---
title: Performing
intro: Retune while playing, loop movements and synchronize to MIDI
---

<!-- SPDX-License-Identifier: GPL-3.0-only -->

### Change a chord without a pitch slide {#retune}

*Silence drone II, retune it, then bring it back in.*

1. Hold **SEL** and tap **OCT+** to fade drone II out.
2. Tap **HOME** until **Pitch** appears. Turn KNOB 3 to the new pitch for drone II.
3. Use **SEL + OCT+** again to fade it back in at the new pitch.

The fade duration is set by **GLO → Setup 1 → Fade** (KNOB 1).

> **Screenshot needed — Pitch with drone II out:** Capture the v1.5 Pitch page with a new drone-II interval.

### Loop your movements {#loop}

*Record and repeat knob or gesture movements.*

1. Press **REC** to begin recording.
2. Move knobs, use gestures or load scenes.
3. Press REC again: the recorded movement repeats while you can play on top.
4. Press REC a third time to stop the loop.

The loop records parameter and original-gesture performance changes; it does not replace the independent **SEL + PLAY** pitch-sequencer transport. STRIP, DARK, STILL and TAIL are live session controls, not scene-slot settings. Test their interaction with REC rather than assuming that a saved scene will restore those amounts.

> **Screenshot needed — REC and LOOP:** Capture the v1.5 header during recording and looping.

### Slow one drone without muting it {#still}

*STILL changes a drone's local time progression, not its audio output.*

1. Start a two-drone progression with **SEL + PLAY**.
2. Hold white key **7 (STILL II)** to slow drone II's LFO, Swell and sequencer clocks. At 100% they stop moving while its oscillator continues sounding.
3. Release to retain that depth. Tap key 7 to return to zero over Morph Time, letting its local clocks smoothly rejoin their master phases. Drone I continues independently.

> **Screenshot needed — STILL II:** Capture the SEQ and Swell progress markers for independently moving drone clocks.

### Edit another page without leaving your place {#peek}

*Temporarily change another page, then return.*

1. Hold a page button, such as **FX**, to peek at that page.
2. Turn a parameter knob while the button is held.
3. Release the button to return to your previous page.

**Hold HOME** shows the activity atlas instead of a normal parameter peek. **Hold ARP** switches Swell on/off.

> **Screenshot needed — HOME atlas:** `build/scenarios/volcano-ui/geological-atlas.png`.

### Play in time with a MIDI clock {#clock}

*Follow an external clock from a DAW or drum machine.*

1. Send MIDI clock from a DAW or drum machine using the FM-1's supported USB or TRS MIDI input.
2. Set **Sync** on under **GLO → Setup 1** (KNOB 4).
3. Open **ARP → Swell** and choose a Division with KNOB 1. The rhythmic movement follows the external clock.

The LFO can also follow MIDI clock; adjust its division with **SEL + SELECT**. See [MIDI](../reference/midi.md).

> **Screenshot needed — MIDI-synced Swell:** Capture the Swell page while an external clock is running.

> **Audio examples pending:** The older recipe renders were made for a different navigation layout and should be regenerated with verified Volcano v1.5 scenarios.
