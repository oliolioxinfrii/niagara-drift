# VOLCANO v1.5 — geological UI overhaul

Status: **web-emulator review build; physical FM-1 performance remains unverified.**
The v1.4 synthesis, scene memory, sequencing, and encoder assignments remain unchanged.
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
  off-white, not warning red. Categorical and bipolar controls retain their
  original selection/zero semantics.
- Graphs showing pitch over time **remain pitch graphs**, not simulated
  seismogram amplitudes.

## Page mapping

| Button | Existing views | New graphical treatment |
| --- | --- | --- |
| HOME | Drones, Pitch | Full-width volcanic cross-section and event-like seismic trace on Drones; distinct, buckling, nine-band geological sections on Pitch |
| ENV | Life, Tone | Volcano/plume observatory; separate per-drone ash/particulate fans |
| EDIT | Osc, Sub · Drive | Dual, dense stratigraphic sections with isolated seams, texture, subharmonic intrusions, and distinct source hues |
| LFO | Waves, Sends, Mods | Existing informative wave/route diagrams, recolored in the Deep Mantle family |
| SEQ | Partner, Pitch moves, Sequence, Intervals, Transitions | Existing note, event, and segment diagrams; warm Crust palette, actual pitch trajectories retained |
| ARP | Swell, Swell shape | Existing two-envelope plots, warm Crust palette |
| FX | Space, Details | Two cyan seismograph-inspired channels: discrete delay arrivals above a decaying reverberant coda, with off-white saturation highlights |
| GLO | Setup 1, Setup 2, Speaker | A compact planetary cutaway: muted observation meridians and warm internal arcs; the standard four-parameter readout remains |
| SAVE | Browser, rename, overwrite confirmation | Orbital scene nodes and satellite; browser retains four-encoder assignments |

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
- `firmware/src/app/ui.c`: procedural, textured VOLCANO wake screen and message
  colors; DSP, parameter selection, and persistence untouched.
- `tests/scenarios/volcano-ui.omni`: host screenshot smoke scenario.

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
