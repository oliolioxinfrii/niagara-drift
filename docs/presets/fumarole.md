# FUMAROLE — original Volcano intro preset

This is a **style-oriented synthesis study**, not a transcription of
Nicolas Godin's **"Acid Lake"** from *Fire of Love*. It was designed
after analysis of the user-supplied recording. The opening has a strong
low-frequency A-centered foundation (~55/110 Hz); upper components and
greater dissonance become prominent over roughly the first 10–20 seconds.
The preset deliberately uses **different pitches**, event intervals,
and sound-shaping decisions while retaining the compositional ideas:
a low stationary bed, gradually shifting dissonance, slow spectral
movement and an uncertain boundary between texture and pulse.

## Audition directly in the firmware / browser emulator

1. Press and hold **SEL**.
2. Tap **REC**, then release SEL. The display announces **FUMAROLE**.
   This loads a scratch sound and a four-position authored sequence,
   selects GHOST mode 11, and clears the two transient output-monitor
   cuts so the complete patch is audible.
   **It does not overwrite any factory preset or stored A–F scene.**
   Ordinary REC without SEL still records Motion Loops.
3. Press **PLAY** for the sound. Allow approximately eight seconds
   of the low drone to establish the atmosphere.
4. Press **SEL + PLAY** to start the prepared, sixty-second pitch
   progression. The transport is initially stopped on purpose.
5. To compare alternatives, use **SEL + PRESETS** to choose another
   GHOST algorithm. The demo initially selects mode **11**, the
   Cross-Synth / Resonant Body / Coupled Feedback blend.
6. To retain the sound and progression, use the existing SAVE browser
   to store them in a **user A–F scene**. The GHOST selector itself is
   currently RAM-only and **will not be saved with the scene**; after
   recalling the scene, select mode 11 again with SEL + PRESETS.

The ordinary factory P1–P16 sounds and any saved user scenes are
untouched. The audition temporarily turns **Pitch Lock off** so the
two drone pitch cursors can move independently. Other room-level
settings remain as they were.

## Sound design

| Element | FUMAROLE settings / function |
|---|---|
| Drone I | G2, 98 Hz, with a quiet G1 sine sub, mostly sine/triangle |
| Drone II | D3, 147 Hz, triangle-oriented, slightly brighter filter |
| Filtering | Dark low-pass drones, asymmetric slow filter-LFO motion |
| Movement | Relatively low drift, subtle oscillator pitch motion |
| Swell | Gentle global rhythmic breath, 18% pulse depth |
| Space | Hall reverb, fairly long dark tail; restrained clean echoes |
| GHOST MIX | 62/100 — moderately GHOST-forward but parents still audible |
| GHOST mode | 11, **Cross-Synth 55% / Resonant Body 30% / Feedback 15%** |
| Existing global feedback | 0, to retain independent control of the interaction |

The GHOST system requires both parent drones to keep sounding,
including when they are attenuated by GHOST MIX.

### Authored one-minute pitch progression

The four positions are interpolated according to the existing
`seq_drift.c` controls; they are not conventional short, retriggered
notes. Each duration below is the time *until* the next position.
The final interval returns to the first position.

| Position | Drone I | Drone II | Interval to next |
|---|---|---|---:|
| 1 | G2 | D3 | 14 s |
| 2 | G2 | E♭3 | 17 s |
| 3 | G2 | B2 | 13 s |
| 4 | F♯2 | C♯3 | 16 s |

The deliberately late-biased interpolation shapes
`{-70, -60, -30, -75}` concentrate changes toward the ends of
the four intervals, creating stretches of relative stillness and
more concentrated harmonic shifts. The upper drone has a 4% phase
offset; the lower drone functions largely as a tonal anchor.

This is an **original harmonic design** on G rather than the
recording's A-centered tonal foundation. It avoids recreating the
recording's exact pitch succession and rhythmic events.

## Source and test locations

- `firmware/src/app/project.c`: `project_fumarole_build()`, exact
  patch parameters and four-node progression.
- `firmware/src/app/ui.c`: explicit **SEL + REC** scratch-load gesture.
- `tests/scenarios/fumarole.omni`: control, state and non-destructive
  behavior regression.
- `tests/host/project_test.c`: validates parameter limits, sequence
  duration, and non-mutation of stored scenes.
- `examples/fumarole-intro.omni`: an optional 70-second host
  performance/rendition; first eight seconds hold the sound before
  starting the sequence.

To render the full example after `sh host/build_host.sh`:

```bash
mkdir -p build/fumarole
build/host/omni_host examples/fumarole-intro.omni build/fumarole
```

This creates `build/fumarole/fumarole-intro.wav`. As with every
experimental GHOST branch change, compile and run the tests on Ubuntu
before treating this as a verified hardware firmware build.
