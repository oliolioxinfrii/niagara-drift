#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Compare Volcano drone kernel variants against the reference path.

Builds host/drone_render.c twice with the same compiler/options, then compares
the raw WAV bytes from every listening scene. The default optimizations are
intended to keep the sound sample-identical. OM_BLEP_RECIP=1 is experimental
and is not included in the default comparison.
"""
import argparse
import hashlib
import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
BASE_FLAGS = [
    "-O2", "-ffp-contract=off", "-std=c99",
    "-Wall", "-Wextra", "-Wno-missing-field-initializers", "-Werror",
    "-DOM_HOST", "-Ifirmware/src/dsp",
]
REFERENCE = [
    "-DOM_NOISE_CHUNK=0", "-DOM_FILTER_SPECIALIZE=0",
    "-DOM_OSC_RUNS=0", "-DOM_BLEP_RECIP=0",
]
DEFAULT = [
    "-DOM_NOISE_CHUNK=1", "-DOM_FILTER_SPECIALIZE=1",
    "-DOM_OSC_RUNS=1", "-DOM_BLEP_RECIP=0",
]


def run(cmd):
    p = subprocess.run(cmd, cwd=ROOT, text=True,
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if p.returncode:
        print(p.stdout, file=sys.stderr)
        raise RuntimeError(f"command failed ({p.returncode}): {' '.join(map(str, cmd))}")


def build(out, flags):
    cc = shlex.split(os.environ.get("CC", "cc"))
    run([*cc, *BASE_FLAGS, *flags, "-o", str(out),
         "host/drone_render.c", "firmware/src/dsp/drone.c", "-lm"])


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--reciprocal", action="store_true",
                    help="compare the experimental BLEP reciprocal path (not bit-identical by design)")
    args = ap.parse_args()
    candidate = [*DEFAULT]
    if args.reciprocal:
        candidate[-1] = "-DOM_BLEP_RECIP=1"
    with tempfile.TemporaryDirectory(prefix="volcano-dsp-") as tmp:
        root = Path(tmp)
        a = root / "reference"
        b = root / "optimized"
        a.mkdir()
        b.mkdir()
        build(root / "ref", REFERENCE)
        build(root / "opt", candidate)
        run([str(root / "ref"), str(a)])
        run([str(root / "opt"), str(b)])
        ref = {f.name: digest(f) for f in a.glob("*.wav")}
        opt = {f.name: digest(f) for f in b.glob("*.wav")}
        if not ref or ref.keys() != opt.keys():
            raise RuntimeError("WAV outputs are empty or scene sets differ")
        different = sorted(k for k in ref if ref[k] != opt[k])
        print(f"Compared {len(ref)} listening scenes.")
        if different:
            print("Different samples:", ", ".join(different))
            if args.reciprocal:
                print("This variation is expected to change float rounding; review the audio differences.")
                return 0
            return 1
        print("All WAV files byte-identical to the reference DSP.")
        return 0


if __name__ == "__main__":
    sys.exit(main())
