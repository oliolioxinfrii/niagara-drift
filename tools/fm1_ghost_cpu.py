#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Measure CPU1 GHOST mode-14 jobs, misses, faults and processing times over USB MIDI.

Usage: python3 tools/fm1_ghost_cpu.py 15
Requires mido and python-rtmidi, and a firmware built with OM_GHOST_CPU1=1.
"""
import os
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fm1_install import PORT_RE, MidoBackend, pack7, pair_output, unpack7

QUERY = b"\xF0" + pack7(bytes([0x00, 0x59, 0x7D, 0, 0, 0, 0xFF])) + b"\xF7"
NAMES = ("active", "jobs", "missed", "faults", "mean_us", "max_us", "pending", "budget_us")


def parse(pkt):
    d = unpack7(pkt[1:-1])
    if len(d) != 39 or d[:3] != b"\x00\x59\x7D" or (~sum(d[6:38]) & 0xFF) != d[38]:
        return None
    w = [int.from_bytes(d[6 + 4*i:10 + 4*i], "little") for i in range(8)]
    return dict(zip(NAMES, w))


def main():
    secs = float(sys.argv[1]) if len(sys.argv) > 1 else 10.0
    be = MidoBackend()
    ins = [n for n in be.input_names() if PORT_RE.search(n)]
    if not ins:
        sys.exit("fm1_ghost_cpu: no FM-1 MIDI input")
    out = pair_output(ins[0], [n for n in be.output_names() if PORT_RE.search(n)])
    link = be.open(ins[0], out)
    first, last = None, None
    try:
        end = time.monotonic() + secs
        while time.monotonic() < end:
            link.drain()
            link.send(QUERY)
            t = time.monotonic() + 1.0
            while (left := t - time.monotonic()) > 0:
                pkt = link.read(left)
                result = pkt and parse(pkt)
                if result:
                    first = first or result
                    last = result
                    break
            time.sleep(0.25)
    finally:
        link.close()
    if not last:
        sys.exit("fm1_ghost_cpu: no CPU1 response (is OM_GHOST_CPU1=1?)")
    print(f"CPU1 online       {bool(last['active'])}")
    print(f"CPU1 jobs         {last['jobs'] - first['jobs']} submitted")
    print(f"CPU1 processing   mean {last['mean_us']} us; peak {last['max_us']} us")
    print(f"Audio deadline    {last['budget_us']} us per 256-frame block")
    print(f"Missed handoffs   {last['missed'] - first['missed']}")
    print(f"CPU1 faults       {last['faults'] - first['faults']}")
    print(f"Outstanding job   {bool(last['pending'])}")
    if last["max_us"] >= last["budget_us"] or last["missed"] > first["missed"] or last["faults"] > first["faults"]:
        print("WARNING: the two-core GHOST probe missed a realtime criterion")


if __name__ == "__main__":
    main()
