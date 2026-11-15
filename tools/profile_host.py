#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Compare host wall time for matched slow- and fast-modulation workloads.

    sh host/build_host.sh
    python3 tools/profile_host.py --repeats 5

This measures the host simulator, not the FM-1's audio ISR load or LCD latency.
Use fm1_cpu.py for on-device audio load and perf/callgrind to locate host costs.
"""
import argparse
import re
import statistics
import subprocess
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PATTERN = re.compile(
    r"host: (\d+) ms simulated, (\d+) blits, render ([\d.]+)% of real time"
)


def run(exe, profile, output):
    output.mkdir(parents=True, exist_ok=True)
    t0 = time.perf_counter()
    proc = subprocess.run(
        [str(exe), str(profile), str(output)],
        cwd=ROOT, capture_output=True, text=True, check=False
    )
    elapsed = time.perf_counter() - t0
    if proc.returncode:
        raise RuntimeError(
            f"{profile.name}: host exited {proc.returncode}\n{proc.stdout}\n{proc.stderr}"
        )
    match = PATTERN.search(proc.stdout)
    if not match:
        raise RuntimeError(f"{profile.name}: missing host statistics\n{proc.stdout}")
    sim_ms, blits, audio_percent = match.groups()
    return elapsed, int(sim_ms), int(blits), float(audio_percent)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repeats", type=int, default=5)
    args = parser.parse_args()
    if args.repeats < 1:
        parser.error("--repeats must be at least 1")
    exe = ROOT / "build/host/omni_host"
    if not exe.is_file():
        parser.error("build/host/omni_host is missing; run sh host/build_host.sh")
    results = {"slow": [], "fast": []}
    for i in range(args.repeats):
        # Alternate order to reduce systematic warm-cache / background-load bias.
        order = ("slow", "fast") if i % 2 == 0 else ("fast", "slow")
        for name in order:
            profile = ROOT / f"tools/profiles/{name}-mods.omni"
            output = ROOT / f"build/profile/{name}"
            results[name].append(run(exe, profile, output))

    print(f"Host simulator, {args.repeats} runs per workload")
    print("Workload    Wall seconds       Simulated s   Blits   Audio render*")
    for name in ("slow", "fast"):
        samples = results[name]
        times = [r[0] for r in samples]
        sim = [r[1] for r in samples]
        if len(set(sim)) != 1:
            raise RuntimeError(f"{name}: simulated durations differed: {sim}")
        print(f"{name:8} {statistics.mean(times):12.3f} "
              f"{sim[0] / 1000.0:12.2f} {samples[0][2]:7} "
              f"{statistics.mean(r[3] for r in samples):13.2f}%")
    slow = results["slow"][0][1]
    fast = results["fast"][0][1]
    if slow != fast:
        print(f"WARNING: simulated durations differ ({slow} vs {fast} ms); "
              "do not interpret the wall-time ratio as normalized.")
    else:
        ratio = statistics.mean(r[0] for r in results["fast"]) / statistics.mean(
            r[0] for r in results["slow"]
        )
        print(f"Fast / slow host wall-time ratio: {ratio:.3f}x")
    print("*Audio render is host dr_render time / its audio budget, not device CPU.")
    print("Run on the same machine with the same build; compare commits separately.")


if __name__ == "__main__":
    main()
