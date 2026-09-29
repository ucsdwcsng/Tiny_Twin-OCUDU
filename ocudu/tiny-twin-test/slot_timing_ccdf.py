#!/usr/bin/env python3
"""
Compare gNB slot timing captures (OCUDU_TTI_LOG_PATH CSV) across runs.

Each input is a CSV written by the MAC slot timing recorder:
    slot_count,enqueue_ns,start_ns,sched_done_ns,results_done_ns

For every run, over an equal-duration window, the script reports:
- slot pacing:        difference of consecutive enqueue_ns (nominal slot duration)
- processing latency: results_done_ns - enqueue_ns (tick -> all results handed to the PHY)
  split into queueing (start - enqueue), scheduling (sched_done - start) and
  PHY request assembly (results_done - sched_done)
- skipped slots:      gaps in slot_count

and plots the CCDF of pacing and processing latency.

Example:
    python slot_timing_ccdf.py off=logs/slot_timing_off.csv on=logs/slot_timing_on.csv --warmup-sec 1

A run given as LABEL=PATH@SEC skips SEC seconds of that run instead of --warmup-sec (e.g. to skip each
run's own UE attach time).
"""

import argparse

import numpy as np


COLUMNS = ["slot_count", "enqueue_ns", "start_ns", "sched_done_ns", "results_done_ns"]


def load(path):
    data = np.genfromtxt(path, delimiter=",", names=True, dtype=np.int64)
    missing = [c for c in COLUMNS if c not in data.dtype.names]
    if missing:
        raise ValueError(f"{path}: missing columns {missing}; is this an old-format TTI file?")
    if data.size < 2:
        raise ValueError(f"{path}: need at least two slots")
    return data


def select_window(data, warmup_sec, duration_sec):
    t = data["enqueue_ns"]
    start_ns = t[0] + int(warmup_sec * 1e9)
    end_ns = start_ns + int(duration_sec * 1e9)
    selected = data[(t >= start_ns) & (t <= end_ns)]
    if selected.size < 2:
        raise ValueError(f"Too few slots with warmup={warmup_sec}s, duration={duration_sec}s")
    return selected


def metrics_ms(data):
    to_ms = lambda ns: ns.astype(np.float64) / 1e6
    return {
        "pacing": to_ms(np.diff(data["enqueue_ns"])),
        "latency": to_ms(data["results_done_ns"] - data["enqueue_ns"]),
        "queueing": to_ms(data["start_ns"] - data["enqueue_ns"]),
        "scheduling": to_ms(data["sched_done_ns"] - data["start_ns"]),
        "assembly": to_ms(data["results_done_ns"] - data["sched_done_ns"]),
    }


def ccdf(samples):
    x = np.sort(samples)
    return x, (len(x) - np.arange(len(x))) / len(x)


def print_stats(label, data, m, duration_sec):
    slot_gaps = np.diff(data["slot_count"].astype(np.int64))
    skipped = int(np.sum(slot_gaps[slot_gaps > 1] - 1))

    print(f"\n{label}")
    print("-" * len(label))
    print(f"Slots            : {data.size} in {duration_sec:.3f} s")
    print(f"Skipped slots    : {skipped}")
    print(f"{'':17}{'mean':>10}{'p50':>10}{'p99':>10}{'p99.9':>10}{'max':>10}  (ms)")
    for name in ["pacing", "latency", "queueing", "scheduling", "assembly"]:
        s = m[name]
        p50, p99, p999 = np.percentile(s, [50, 99, 99.9])
        print(f"{name:17}{np.mean(s):10.4f}{p50:10.4f}{p99:10.4f}{p999:10.4f}{np.max(s):10.4f}")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("runs", nargs="+", help="LABEL=PATH[@SEC] (or PATH[@SEC], labelled by file name)")
    parser.add_argument("--warmup-sec", type=float, default=0.0, help="discard this many seconds from every run")
    parser.add_argument("--duration-sec", type=float, default=None, help="default: shortest run after warmup")
    parser.add_argument("--output", default="slot_timing_ccdf.png")
    args = parser.parse_args()

    runs = []
    for spec in args.runs:
        label, _, path = spec.rpartition("=")
        path, _, skip = path.partition("@")
        runs.append((label or path, load(path), float(skip) if skip else args.warmup_sec))

    available = min((d["enqueue_ns"][-1] - d["enqueue_ns"][0]) / 1e9 - skip for _, d, skip in runs)
    if available <= 0:
        raise ValueError("Warmup is longer than one of the runs")
    duration_sec = available if args.duration_sec is None else args.duration_sec
    if duration_sec > available:
        raise ValueError(f"Requested duration {duration_sec:.3f}s exceeds common duration {available:.3f}s")

    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    fig, (ax_pacing, ax_latency) = plt.subplots(1, 2, figsize=(12, 5))
    for label, data, skip in runs:
        window = select_window(data, skip, duration_sec)
        m = metrics_ms(window)
        print_stats(label, window, m, duration_sec)
        ax_pacing.step(*ccdf(m["pacing"]), where="post", label=label)
        ax_latency.step(*ccdf(m["latency"]), where="post", label=label)

    ax_pacing.set(xlabel="Slot inter-arrival (ms)", title="Slot pacing")
    ax_latency.set(xlabel="Slot tick → results to PHY (ms)", title="MAC slot processing latency")
    for ax in (ax_pacing, ax_latency):
        ax.set(yscale="log", ylabel="CCDF  P(X > x)")
        ax.grid(True, which="both", alpha=0.3)
        ax.legend()
    fig.tight_layout()
    fig.savefig(args.output, dpi=150)
    print(f"\nSaved {args.output}")


if __name__ == "__main__":
    main()
