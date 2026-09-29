#!/usr/bin/env python3
"""Summarize the gNB's DL scheduling decisions per UE over time windows, from the OCUDU log on stdin.

Used by fixed_policy_test.sh:  docker exec ocudu_gnb cat /tmp/gnb.log | analyze_scheduler_log.py WINDOW...
Each WINDOW is "name|sched_policy|mcs_policy|start|end|cap0,cap1" with ISO timestamps (UTC, as in the log).

Per UE it prints the newTx DL grants the scheduler made ("Slot decisions ... DL: ue=N c-rnti=... rb=[a..b) ...
newtx=true ... tbs=T"), their RB sizes, the share within the window's PRB cap, the scheduled DL rate (sum of TBS
bytes) and the modulation of the first transmissions from the PHY "PDSCH: rnti=... mod=... rv=0" lines.
"""

import re
import sys
from collections import defaultdict
from datetime import datetime

DL = re.compile(r"DL: ue=(\d+) c-rnti=(0x[0-9a-f]+) .*?rb=\[(\d+)\.\.(\d+)\).*?newtx=(true|false) .*?tbs=(\d+)")
PDSCH = re.compile(r"PDSCH: rnti=(0x[0-9a-f]+) .*?mod=(\w+) rv=(\d+)")


def main():
    windows = []
    for arg in sys.argv[1:]:
        name, sched, mcs, start, end, caps = arg.split("|")
        windows.append((name, sched, mcs, start, end, [int(c) for c in caps.split(",")]))
    stats = {w[0]: defaultdict(lambda: {"n": 0, "rbs": [], "tbs": 0, "mod": defaultdict(int)}) for w in windows}
    rnti_to_ue = {}
    for line in sys.stdin:
        ts = line[:26]
        for name, _, _, start, end, _ in windows:
            if not start <= ts <= end:
                continue
            if "Slot decisions" in line:
                for ue, rnti, first, last, newtx, tbs in DL.findall(line):
                    rnti_to_ue[rnti] = int(ue)
                    if newtx == "true":
                        s = stats[name][int(ue)]
                        s["n"] += 1
                        s["rbs"].append(int(last) - int(first))
                        s["tbs"] += int(tbs)
            elif "PDSCH:" in line:
                m = PDSCH.search(line)
                if m and m.group(3) == "0" and m.group(1) in rnti_to_ue:
                    stats[name][rnti_to_ue[m.group(1)]]["mod"][m.group(2)] += 1
            break

    print(f"{'phase':6}{'UE':>3}{'cap':>5}{'newTx':>7}{'meanRB':>8}{'maxRB':>6}{'<=cap':>7}{'DL Mbps':>9}"
          f"  PDSCH modulation of first transmissions")
    for name, sched, mcs, start, end, caps in windows:
        seconds = (datetime.fromisoformat(end) - datetime.fromisoformat(start)).total_seconds()
        for ue in sorted(stats[name]):
            s = stats[name][ue]
            n, rbs = s["n"], s["rbs"]
            cap = caps[ue] if ue < len(caps) else 0
            within = 100.0 * sum(r <= cap for r in rbs) / n if n else 0.0
            total = sum(s["mod"].values())
            mods = " ".join(f"{k} {100.0 * v / total:.0f}%" for k, v in sorted(s["mod"].items(), key=lambda kv: -kv[1]))
            print(f"{name:6}{ue:>3}{cap:>5}{n:>7}{(sum(rbs) / n if n else 0):>8.1f}{(max(rbs) if rbs else 0):>6}"
                  f"{within:>6.0f}%{s['tbs'] * 8 / seconds / 1e6:>9.1f}  {mods}")
        print(f"       sched {sched}, mcs {mcs}")


if __name__ == "__main__":
    main()
