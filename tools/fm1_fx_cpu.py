#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Measure CPU1 GHOST+FX pipeline jobs, misses, faults and execution times over USB MIDI.

Usage: python3 tools/fm1_fx_cpu.py 15
Requires mido and python-rtmidi, and a firmware built with OM_FX_CPU1=1.
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
        sys.exit("fm1_fx_cpu: no FM-1 MIDI input")
    out = pair_output(ins[0], [n for n in be.output_names() if PORT_RE.search(n)])
    link = be.open(ins[0], out)
    first, last = None, None
    retirement = None
    online_before = None
    began = time.monotonic()
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
                    if first is None:
                        first = result
                    if online_before and not result["active"] and retirement is None:
                        retirement = (time.monotonic() - began, result.copy())
                    online_before = bool(result["active"])
                    last = result
                    break
            time.sleep(0.25)
    finally:
        link.close()
    if not last:
        sys.exit("fm1_fx_cpu: no CPU1 response (is OM_FX_CPU1=1?)")
    print(f"CPU1 online       {bool(last['active'])} (at first poll: {bool(first['active'])})")
    print(f"CPU1 jobs         {last['jobs'] - first['jobs']} submitted ({last['jobs']} since bind)")
    print(f"CPU1 processing   mean {last['mean_us']} us; peak {last['max_us']} us")
    print(f"Audio deadline    {last['budget_us']} us per 256-frame block")
    print(f"Missed handoffs   {last['missed'] - first['missed']} during probe ({last['missed']} since bind)")
    print(f"CPU1 faults       {last['faults'] - first['faults']} during probe ({last['faults']} since bind)")
    print(f"Outstanding job   {bool(last['pending'])}")
    if retirement is not None:
        t, r = retirement
        print(f"CPU1 retirement   observed at ~{t:.1f} s "
              f"(cumulative misses={r['missed']}, faults={r['faults']})")
    elif not first["active"]:
        print("CPU1 retirement   already offline at first poll (event may predate probe)")
    if not last["active"] or last["max_us"] >= last["budget_us"] or last["missed"] or last["faults"]:
        print("WARNING: CPU1 offline, retired, or previously missed a realtime criterion")
        if last["missed"]:
            print("Hint: missed handoff; CPU1 was still busy at the next audio block")
        elif last["faults"]:
            print("Hint: worker submission/wait failure; cause needs firmware diagnostics")


if __name__ == "__main__":
    main()
