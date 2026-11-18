#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Measure Frasil-inspired Volcano DSP paths against the unmodified kernels.

Use Linux Valgrind/Callgrind to compare host instruction counts for identical
drone_cost test cases, with controlled compiler flags. This is *not* an FM-1
cycle estimate; use hardware timing to select winners for the Jieli CPU.

    python3 tools/dsp_cost_variants.py --cases 4
    python3 tools/dsp_cost_variants.py --cases 3,4,5
"""
import argparse
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SAMPLES = int(2.5 * 44100 / 256) * 256  # Callgrind counts the full cost() call
FLAGS = [
    "-O2", "-g", "-ffp-contract=off", "-std=c99",
    "-D_POSIX_C_SOURCE=200809L", "-D_DEFAULT_SOURCE", "-DOM_HOST",
    "-Ifirmware/src/dsp",
]

# All variations use the same compiler, application, settings, and test cases.
# Off is the exact non-specialized code path, including original noise state.
VARIANTS = {
    "reference": (0, 0),
    "noise": (1, 0),
    "filter": (0, 1),
    "default": (1, 1),
}


def cases_from_source():
    src = (ROOT / "tests/host/drone_cost.c").read_text()
    match = re.search(r"names\[\]\s*=\s*\{(.*?)\};", src, re.S)
    if not match:
        raise RuntimeError("could not find drone_cost's case names")
    return re.findall(r'"([^"]+)"', match.group(1))


def run(cmd):
    p = subprocess.run(cmd, cwd=ROOT, text=True,
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if p.returncode:
        raise RuntimeError(f"command failed ({p.returncode}): "
                           f"{' '.join(map(str, cmd))}\n{p.stdout[-3000:]}")


def compile_variant(name, flags, output):
    cc = shlex.split(os.environ.get("CC", "cc"))
    if not cc:
        raise RuntimeError("CC was empty")
    defs = [
        f"-D{key}={int(value)}"
        for key, value in zip(("OM_NOISE_CHUNK", "OM_FILTER_SPECIALIZE"), flags)
    ]
    run([*cc, *FLAGS, *defs, "-o", str(output),
         "tests/host/drone_cost.c", "firmware/src/dsp/drone.c", "-lm"])


def count(exe, case, output):
    run(["valgrind", "--quiet", "--tool=callgrind",
         "--toggle-collect=cost*", f"--callgrind-out-file={output}",
         str(exe), str(case)])
    data = output.read_bytes()
    match = re.search(rb"^(?:summary|totals):\s*(\d+)", data, re.M)
    if not match:
        raise RuntimeError(f"missing Callgrind instruction count in {output}")
    return int(match.group(1)) / SAMPLES


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", default="3,4,5",
                        help="comma-separated drone_cost case indices (default: 3,4,5)")
    args = parser.parse_args()
    if not shutil.which("valgrind"):
        parser.error("Valgrind is needed. On Ubuntu: sudo apt install valgrind")
    names = cases_from_source()
    try:
        choices = [int(s.strip()) for s in args.cases.split(",")]
    except ValueError:
        parser.error("--cases must be comma-separated integers, e.g. 3,4,5")
    if not choices or any(c < 0 or c >= len(names) for c in choices):
        parser.error(f"case indices must be between 0 and {len(names) - 1}")
    choices = list(dict.fromkeys(choices))
    variants = ["reference", "noise", "filter", "default"]
    result = {}
    with tempfile.TemporaryDirectory(prefix="volcano-dsp-cost-") as temp:
        tempdir = Path(temp)
        for variant in variants:
            exe = tempdir / variant
            compile_variant(variant, VARIANTS[variant], exe)
            for case in choices:
                print(f"Measuring {variant}, case {case}: {names[case]}", flush=True)
                result[(variant, case)] = count(exe, case, tempdir / f"{variant}-{case}.cg")

    print("\nHost instructions per sample (Callgrind); change versus reference\n")
    print("| Configuration | " + " | ".join(f"{i}: {names[i]}" for i in choices) + " |")
    print("|---|" + "|".join("---:" for _ in choices) + "|")
    for variant in variants:
        cells = []
        for case in choices:
            instructions = result[(variant, case)]
            base = result[("reference", case)]
            delta = (instructions / base - 1.0) * 100.0 if base else 0.0
            cells.append(f"{instructions:,.0f} ({delta:+.1f}%)")
        print(f"| {variant} | " + " | ".join(cells) + " |")
    print("\nNegative percentages indicate fewer Linux host instructions, "
          "not verified FM-1 CPU savings.")


if __name__ == "__main__":
    try:
        main()
    except RuntimeError as error:
        sys.exit(str(error))
