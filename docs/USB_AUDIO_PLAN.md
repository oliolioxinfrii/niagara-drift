# USB audio routing: development plan

Status: **design and UI scaffold only**. This document describes the
`feature/usb-audio-routing` branch, not a released feature. The tested Swell
and UI baseline remains on `main`.

## Intended use

Connect two physical M-VAVE FM-1 synthesizers:

- **Source:** Niagara Drift generates a stereo mix.
- **Destination:** JIANT firmware accepts USB audio input and mixes it into
  Track 6 when six tracks are active (verified by the user using Android
  Harbor and a computer as USB hosts).
- **Goal:** The source FM-1 supplies that stereo stream directly over USB,
  without requiring a phone or computer between the instruments.
- **Outputs:** Prefer keeping the source's headphone/jack monitoring active
  alongside USB; let JIANT provide final mixing and monitoring.

This is **USB Audio Class**, not MIDI, and requires the source to act as a
USB host while JIANT remains a USB device. Simply enabling the existing
UAC device output does not connect two USB devices.

## Current firmware

- `firmware/src/main.c` sets `OM_UAC=1`.
- `firmware/src/usb.c` implements a class-compliant stereo, 16-bit,
  44.1 kHz USB **device** recording endpoint for a computer/phone host.
- `firmware/src/app/main_fm1.c` taps the existing stereo render via
  `uac_tap()`; analog output remains independent.
- The new **GLO → USB** page is read-only: it reports this device stream
  and explicitly marks host audio as unavailable. No false Host/Audio/Off
  selector is present. Other GLO pages and existing view IDs are preserved.

## Next implementation milestone

1. Verify the available JieLi FM-1 USB host controller mode, USB power
   and cabling requirements, and whether the host-mode driver can coexist
   with the audio render and UI scan deadlines.
2. Enumerate JIANT's USB audio configuration on real hardware, including
   its isochronous OUT endpoint, supported rates, channels and feedback/
   synchronization strategy.
3. Implement a bounded stereo PCM queue and frame scheduler from the
   existing master render to the host endpoint, with disconnection and
   underrun handling. Keep the headphones/jack path active.
4. Preserve the original USB device firmware-update path, with a
   reliable way to return to Device mode. Default to Device mode until
   host behavior and recovery are hardware-tested.
5. Only after the host stream works, replace the status-only GLO page
   with a small, functional choice such as **Device / Audio Host**.
   Avoid JIANT's Stems, IN+OUT and other modes unless the use case
   actually requires them.

## Validation

Run `tests/run_tests.sh` and inspect `tests/scenarios/usb-menu.omni`
and the generated `usb.png` screen before testing on physical hardware.
A host audio transport also needs tests beyond the emulator: endpoint
enumeration, feedback/clock drift, latency, USB disconnect/reconnect,
startup without JIANT, power and update-mode recovery.

**No functional direct FM-1-to-FM-1 USB audio has been implemented yet.**
