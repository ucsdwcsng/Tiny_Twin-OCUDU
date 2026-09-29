#!/usr/bin/env python3
"""Generate a 10-tap trace whose first tap decreases linearly."""

from pathlib import Path


OUTPUT = Path(__file__).resolve().parent / "traces" / "real_linear.txt"
LINES = 100_000
MAX_TAP = 2.0
MIN_TAP = 0.1


with OUTPUT.open("w") as trace:
    for index in range(LINES):
        first_tap = MAX_TAP + (MIN_TAP - MAX_TAP) * index / (LINES - 1)
        trace.write(f"{first_tap:.9f} 0 0 0 0 0 0 0 0 0\n")

print(f"Generated {LINES} rows in {OUTPUT}")
