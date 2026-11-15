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
| HOME | Drones, Pitch | Fixed-dimness volcanic strata; Drone I level advances a lava stream from ground to vent to flank, Drone II level increases smoke; two seismic channels show level as amplitude and cutoff as active width. Pitch retains two buckling geological sections |
| ENV | Life, Tone | Volcano/plume observatory; separate per-drone ash/particulate fans |
| EDIT | Osc, Sub · Drive | Native Niagara-style waveform bundles, morphed oscillator shapes, subharmonic blend and drive response; updated source hues, header and thermal meters |
| LFO | Waves, Sends, Mods | Existing informative wave/route diagrams, recolored in the Deep Mantle family |
| SEQ | Partner, Pitch moves, Sequence, Intervals, Transitions | Existing note, event, and segment diagrams; warm Crust palette, actual pitch trajectories retained |
| ARP | Swell, Swell shape | Existing two-envelope plots, warm Crust palette |
| FX | Space, Details | Stronger cyan seismographic arrivals and reverberant coda, using perceptual gain at low wet mixes and large amplitude near 100%; icy-white value ramps |
| GLO | Setup 1, Setup 2, Speaker | Planetary cross-section with true tuning ticks and a Tune-responsive A marker; Setup 2 restores animated stereo spacing and exposes only Tuning, Tune and Stereo |
| SAVE | Browser, rename, overwrite confirmation | Orbital scene nodes and satellite; browser retains four-encoder assignments |

## Control behavior

- Setup 2 uses K1 Tuning, K2 Tune, K3 Stereo, and leaves K4 unassigned.
  Pressure uses the former neutral (50) response.
- A factory preset selection or transition clears any previously authored
  RAM-only multistep sequence; user scenes retain their existing behavior.
- The global LFO rate, movement and gesture-time readouts use value-dependent
  dark-red-to-yellow halos around white text.
- The splash screen has a black sky above the separately layered mineral ground.

## ENV visual mappings

- **Feedback:** plume fullness and number of ascent strands.
- **Drift:** windward plume displacement and irregularity (at zero it rises almost vertically).
- **Character:** incandescent activity at the vent.
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
measurements of geological energy. Labels and the button map remain legible
independent of brightness.

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
  response, three-control Setup 2 and one-step factory preset recalls.
- `tests/scenarios/volcano-ui.omni`: host screenshot smoke scenario.
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
`tone.png`, `surface.png`, and `atmosphere.png`.

## Review questions

1. Legibility and overlap of condensed fonts at *actual* 240 × 240 display size.
2. Thermal ramp readability, especially its darkest red.
3. HOME-held atlas timing and whether a press-to-peek visual change is acceptable.
4. Whether the Life page's feedback/drift/character/age mappings are visually
   distinguishable across their ranges.
5. Real FM-1 display update cost, and whether a lower atlas redraw rate is required.

Only the native firmware/web-emulator build and actual FM-1 tests can validate
runtime performance; no physical-device testing is claimed here.
