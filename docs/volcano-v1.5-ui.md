# VOLCANO v1.5 — geological UI overhaul

Status: **web-emulator review build; physical FM-1 performance remains unverified.**
The v1.4 sound engine and scene storage are retained. Setup 2 no longer exposes
pressure sensitivity, and factory preset recalls reset the RAM-only sequencer to one step.
The new drawings are composed at native **240 × 240 pixels** by the existing RGB565
strip renderer; no photograph, full-screen framebuffer, or additional DSP layer is required.

## Design contract

- Persistent near-black background and predominantly warm-white text.
- Layer accent on the **page title, 2-pixel header rule, and 5-rung depth marker**;
  the ordinary parameter labels and numerical readouts remain white or neutral.
- Five layer identities: **Atmosphere** cyan (FX), **Surface** sulfur yellow
  (HOME), **Crust** orange (SEQ and ARP), **Mantle** lava orange-red (EDIT and ENV),
  **Deep Mantle** red (LFO).
- Memory/Orbit (SAVE) and Planet (GLO) are *system* contexts rather than deeper
  sound layers. They use gold/neutral accents and deliberately avoid a
  misleading extra geological-depth rung.
- Continuous-parameter indicators use a dark-red → yellow thermal gradient.
  Numeric values remain readable white. The two audio sources have independent
  sulfur-yellow and lava-red line palettes; peaks and saturated values receive
  off-white, not warning red. FX amounts instead use a blue-to-icy-white scale;
  continuous sliders retain their gradient when no control is selected. Categorical and bipolar controls retain their
  original selection/zero semantics.
- Graphs showing pitch over time **remain pitch graphs**, not simulated
  seismogram amplitudes.

## Page mapping

| Button | Existing views | New graphical treatment |
| --- | --- | --- |
| HOME | Drones, Pitch | Fixed-dimness volcanic strata; Drone I drives lava up and down the left flank, Drone II level increases smoke; two seismographic channels smooth actual scope samples and overlay their respective filter responses, preserving cutoff, mode, and resonance information without repeating artificial pulses. Pitch shows Niagara's oscilloscopic history by default and filter-response strands while Reso is adjusted |
| ENV | Life, Tone | Volcano/plume observatory with Character lava anchored at the bottom of the cone; separate per-drone ash/particulate fans |
| EDIT | Osc, Sub · Drive | Native Niagara-style waveform bundles, morphed oscillator shapes, subharmonic blend and drive response; updated source hues, header and thermal meters |
| LFO | Waves, Sends, Mods | Existing informative wave/route diagrams, recolored in the Deep Mantle family |
| SEQ | Partner, Pitch moves, Sequence, Intervals, Transitions | Existing note, event, and segment diagrams; warm Crust palette, actual pitch trajectories retained. Random Start now randomizes all active transition shapes, including the loop back to step 1 |
| ARP | Swell, Swell shape | Existing two-envelope plots, warm Crust palette |
| FX | Space, Details | Unchanged delay-arrival waveform above a model-driven, three-layer reverb coda. Room, Hall, Cathedral, Chorus Hall and Shimmer now have characteristic onsets and motion. DAMP removes treble texture, LENGTH changes the compressed tail, Chorus Hall adds a counter-moving strand, and Shimmer introduces an octave-up halo with an eight-second wash. The 9.24-second forward/reverse loop and icy-blue palette remain |
| GLO | Setup 1, Setup 2 | Setup 1 gives yellow Fade (top), orange Morph (middle), and red Scene (lower third) distinct plateau heights on a common five-minute maximum Scene axis (or a longer maximum clock-synced span). Pixel-column strokes keep all three curves and their complete plateaus continuous at native and scaled display sizes. Setup 2 retains the same-sized centered planet, with animated L/R wavy orbits converging toward or diverging from a fixed external orbital reference as Stereo changes; the built-in speaker remains on KNOB 4 |
| SAVE | Browser, rename, overwrite confirmation | Orbital scene nodes and satellite; browser retains four-encoder assignments |

## FX reverb seismograph (model-driven, not FFT)

The FX picture retains a seismographic layout within the 240 × 124-pixel
picture strip. Only its **lower, reverb trace** has changed: the upper delay
arrivals are unchanged. This is a **parameter-derived diagram**, not a
measurement of the device's audible spectral output.

The reverb types reuse their actual DSP profile values (mirrored here in
the visualization): Room (4 ms predelay, 0.35 network size), Hall (18 ms,
0.75), Cathedral (42 ms, 1.0), Chorus Hall (24 ms, 0.85 and much stronger
modulation), and Shimmer (Hall geometry with an octave-up input). The left
part of the trace resolves the early predelay and network buildup; the rest
uses a compressed decay representation. This is intentionally not a single
linear time scale, because milliseconds of predelay and seconds of reverb
would otherwise be impossible to read together.

Three overlapping blue traces represent slow body, mid-frequency diffusion
and treble detail. DAMP progressively removes the fine trace toward the tail
while preserving the low-frequency body; at **Inf**, the DSP disables damping
and the visual trace also holds without decay. Chorus Hall draws an additional,
dim counter-moving strand corresponding to its larger delay-line modulation
and stereo width. Shimmer gradually adds a finer icy-blue halo over roughly
eight seconds and lets it recede over six seconds. This wash is estimated
inside the display, not read from the actual audio-buffer level.

All textures remain deterministic, use the existing FX blue ramp and
follow the same cosine-eased 9.24-second forward/reverse scroll as before.
No audio DSP, saved-state layout or parameter mapping has been changed.
`tests/scenarios/fx-reverb-model.omni` records screenshots for all five
types, maximum DAMP, and infinite hold; build and hardware frame-rate
validation are still required.

## Control behavior

- Setup 2 uses K1 Tuning, K2 Tune, K3 Stereo and K4 Speaker On/Off;
  the former third GLO page is retired. Pressure retains its neutral response.
- A factory preset selection or transition clears any previously authored
  RAM-only multistep sequence; user scenes retain their existing behavior.
- Automatic onboarding tip popups are disabled; explicit action messages and
  second-part announcements remain available.
- The global LFO rate, movement and gesture-time readouts use value-dependent
  dark-red-to-yellow halos and a matching gradient ruler around white text. The
  halo's interpolation stays in 24-bit RGB until it is converted to RGB565, so
  intermediate values no longer acquire a spurious green tint.
- The skippable splash takes **14 seconds**, with an initially unlit, text-free volcano. Magma rises from the bottom after one second; the version line (`VOLCANO ` + build version) fades in during the ascent. The eruption starts at about 5.1 seconds, followed by the large `VOLCANO` title at about 6.3 seconds; the former `M-VAVE FM-1` subtitle is omitted. The version fades out from 8.75–10.75 seconds, and the title begins fading halfway through that interval (9.75–11.75 seconds). Finally, the complete geological illustration fades to the dark sky between 12 and 14 seconds before HOME appears. All text is alpha-blended into the artwork rather than dimmed toward black, and **any button/key still skips startup**.

## ENV visual mappings

- **Feedback:** plume fullness and number of ascent strands.
- **Drift:** windward plume displacement and irregularity (at zero it rises almost vertically).
- **Character:** lava rises through a vent and progressively flows over the left flank, with incandescent spatter.
- **Age:** deterministic fine ash grains/weathering. Random particles are not generated at audio rate.
- **Tone / Noise:** the two drones have separate ash fields; Noise controls particle
  count and Color controls the distribution, with WHITE/PINK still written explicitly.

## HOME-held atlas

A normal **HOME tap** keeps the existing behavior. **Hold HOME for 400 ms**
(excluding SEL+HOME) to view a temporary geological atlas. The atlas is not a
new button-driven navigation mode. Release HOME to return to the previous
page, using the existing hold/peek logic. It updates at approximately 10 Hz
with an immediate first render.

The five strata remain visible when inactive. Their brightness derives from:

- Atmosphere: maximum of delay/reverb wet mix.
- Surface: drone levels multiplied by the global fade.
- Crust: currently evaluated Swell envelope, or sequencer-active fallback.
- Mantle: Feedback, Drift, and Character settings.
- Deep Mantle: magnitude of the active LFO signals scaled by Movement.

These are **instrument-state indicators**, not scientifically calibrated
measurements of geological energy. The atlas now places cyan atmospheric particles above the surface volcano,
with the dense warm rock strata and magma feeder entirely beneath it. The
labels and button map remain legible independent of brightness.

## Implementation locations

- `firmware/src/app/volcano_look.c`: layer palette and shared brightening, normalization,
  and depth-marker helpers.
- `firmware/src/app/volcano_geometry.c`: procedural stratigraphic panels, volcanic
  cross-sections, plume, ash, seismographic traces and planetary cutaway.
- `firmware/src/app/volcano_system.c`: Orbit scene browser, overwrite/rename panels,
  and activity-driven HOME navigation atlas.
- `firmware/src/app/look.c`: existing UI renderer, page dispatch, depth-coded
  header and intensity meters. Source-gradient rendering is independent from
  intensity meters.
- `firmware/src/app/ui.c`: black-sky layered VOLCANO wake, neutral pressure
  response, Speaker on Setup 2 KNOB 4 and one-step factory preset recalls.
- `tests/scenarios/volcano-ui.omni`: host screenshot smoke scenario, including
  Pitch scopes, touched-resonance filter curves and Setup time overlays.
- `tests/scenarios/sequence-random.omni`: confirms non-sine randomized
  interpolations and captures the transition curves.
- `tests/scenarios/volcano-presets.omni`: factory-preset one-step sequence regression.

## Emulator review

From a checkout of `feature/volcano-v1.5-ui`:

```sh
git pull --ff-only
sh tests/run_tests.sh
sh web/emu/build.sh
python3 -m http.server 8766 --directory build/emu
```

The test runner writes the new gallery of screenshots to
`build/scenarios/volcano-ui/`, including
`geological-atlas.png`, `orbit-memory.png`, `life.png`,
`tone.png`, `surface.png`, `pitch-scope.png`, `pitch-resonance.png`,
`setup-time-overlay.png`, `planet-stereo-narrow.png`, `planet-stereo-wide.png`,
`life-lava-full.png`, and `atmosphere.png`.

## Review questions

1. Legibility and overlap of condensed fonts at *actual* 240 × 240 display size.
2. Thermal ramp readability, especially its darkest red.
3. HOME-held atlas timing and whether a press-to-peek visual change is acceptable.
4. Whether the Life page's feedback/drift/character/age mappings are visually
   distinguishable across their ranges.
5. Real FM-1 display update cost, and whether a lower atlas redraw rate is required.

Only the native firmware/web-emulator build and actual FM-1 tests can validate
runtime performance; no physical-device testing is claimed here.
