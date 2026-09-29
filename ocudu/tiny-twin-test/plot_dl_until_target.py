#!/usr/bin/env python3
"""Plot DL SINR and RSRP up to the first 41.4 dB / -40 dBm report."""

import re
import sys

import matplotlib.pyplot as plt


if len(sys.argv) != 2:
  raise SystemExit(f"Usage: {sys.argv[0]} UE_LOG_FILE")

pattern = re.compile(r"DL Chan:.*?SINR\s+(-?\d+(?:\.\d+)?)\s+dB\s+RSRP\s+(-?\d+(?:\.\d+)?)\s+dBm")
sinr = []
rsrp = []

with open(sys.argv[1], encoding="utf-8", errors="replace") as log:
  for line in log:
    match = pattern.search(line)
    if not match:
      continue

    sinr_value = float(match.group(1))
    rsrp_value = float(match.group(2))
    sinr.append(sinr_value)
    rsrp.append(rsrp_value)

    if sinr_value == 41.4 and rsrp_value == -40:
      break

if not sinr:
  raise SystemExit("No DL Chan measurements found in the log")

if sinr[-1] != 41.4 or rsrp[-1] != -40:
  raise SystemExit("Target SINR 41.4 dB / RSRP -40 dBm was not found")

measurements = range(1, len(sinr) + 1)
figure, axes = plt.subplots(2, 1, sharex=True, figsize=(8, 7))

axes[0].plot(measurements, sinr, marker="o")
axes[0].set_ylabel("SINR (dB)")
axes[0].grid(True)

axes[1].plot(measurements, rsrp, marker="o")
axes[1].set_xlabel("DL Chan measurement")
axes[1].set_ylabel("RSRP (dBm)")
axes[1].grid(True)

figure.tight_layout()
figure.savefig("dl_channel_until_41.4dB.png", dpi=150)
print(f"Saved {len(sinr)} measurements to dl_channel_until_41.4dB.png")
