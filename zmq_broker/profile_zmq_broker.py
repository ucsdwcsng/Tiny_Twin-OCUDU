#!/usr/bin/env python3
"""Profile zmq_broker: service time (DL, UL), throughput, CPU and memory, for one or more UE counts.

The broker measures its own service time and throughput over a window that this script opens with SIGUSR1 and closes
with SIGUSR2 (the broker then prints a "zmq_broker summary:" line and the service-time histograms). CPU (per thread),
memory and context switches are sampled from /proc of the broker process on the host.

  Sweep (runs the testbed through ocudu/tiny-twin-test/run_multi_ue.sh for each UE count):
    profile_zmq_broker.py --ues 1 4 8 16 [--duration 60] [--warmup 10] [-- extra run_multi_ue.sh options]
  Attach to a broker that is already running (e.g. brought up by hand with the compose override):
    profile_zmq_broker.py --duration 30
  Redraw the plots of an earlier profile:
    profile_zmq_broker.py --replot DIR

Output in --out (default ocudu/tiny-twin-test/logs/broker_profile_<ts>/): <N>ue.json per run, profile.csv (one row per
run), broker_profile.png (main figure: service time p50/p99, throughput, CPU and memory vs UE count) and the
supplementary broker_service_ccdf.png (full service-time distributions), broker_cpu_breakdown.png (CPU per thread,
preemption) and broker_timeseries.png (CPU and memory over each window). Run it with the analysis venv, which has matplotlib and numpy:
    ocudu/tiny-twin-test/.venv-tap-sweep/bin/python zmq_broker/profile_zmq_broker.py --ues 1 4 8 16
"""

import argparse
import csv
import datetime
import json
import os
import signal
import subprocess
import sys
import threading
import time

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEST_DIR = os.path.join(REPO, "ocudu", "tiny-twin-test")
RUN_MULTI_UE = os.path.join(TEST_DIR, "run_multi_ue.sh")
CONTAINER = "ocudu_zmq_broker"
# Same gNB settings as run_multi_ue_sweep.sh, so every UE count runs with the same configuration (16 UEs need them).
DEFAULT_RUN_ARGS = ["--csi-period", "80", "--gnb-config", os.path.join(TEST_DIR, "configs", "gnb_multi_ue.yml")]
SLOT_SAMPLES = 11520  # 23.04 Msps, 30 kHz SCS: samples per slot
TARGET_SLOT_MS = 1.0
CLK_TCK = os.sysconf("SC_CLK_TCK")
SAMPLE_PERIOD_S = 0.5


def log(msg):
    print(f"[{datetime.datetime.now():%H:%M:%S}] {msg}", flush=True)


# ---------------------------------------------------------------- broker process -----------------------------------


def broker_pid():
    out = subprocess.run(["docker", "inspect", "-f", "{{.State.Running}} {{.State.Pid}}", CONTAINER],
                         capture_output=True, text=True)
    if out.returncode != 0 or not out.stdout.startswith("true"):
        return None
    return int(out.stdout.split()[1])


def read_threads(pid):
    """{tid: (name, cpu_ticks, voluntary_ctxt, nonvoluntary_ctxt)} from /proc/<pid>/task."""
    threads = {}
    base = f"/proc/{pid}/task"
    for tid in os.listdir(base):
        try:
            with open(f"{base}/{tid}/stat") as f:
                stat = f.read()
            with open(f"{base}/{tid}/status") as f:
                status = dict(line.split(":", 1) for line in f if ":" in line)
        except FileNotFoundError:
            continue  # thread exited
        name = stat[stat.index("(") + 1:stat.rindex(")")]
        fields = stat[stat.rindex(")") + 2:].split()
        ticks = int(fields[11]) + int(fields[12])  # utime + stime
        threads[tid] = (name, ticks, int(status["voluntary_ctxt_switches"]), int(status["nonvoluntary_ctxt_switches"]))
    return threads


def read_memory_kb(pid):
    with open(f"/proc/{pid}/status") as f:
        status = dict(line.split(":", 1) for line in f if ":" in line)
    return int(status["VmRSS"].split()[0]), int(status["VmHWM"].split()[0])


def thread_group(name):
    if name.startswith("ZMQbg/IO"):
        return "zmq_io"
    if name.startswith("ZMQbg"):
        return "zmq_other"
    return "main"


def broker_logs():
    return subprocess.run(["docker", "logs", CONTAINER], capture_output=True, text=True).stdout


def parse_summary(logs):
    """Last 'zmq_broker summary:' line and its histograms from the broker log."""
    lines = logs.splitlines()
    for i in range(len(lines) - 1, -1, -1):
        if lines[i].startswith("zmq_broker summary:"):
            summary = {}
            for kv in lines[i].split(":", 1)[1].split():
                key, value = kv.split("=")
                summary[key] = float(value)
            hists = {}
            for line in lines[i + 1:i + 3]:
                if line.startswith("zmq_broker hist "):
                    direction = line.split()[2].rstrip(":")
                    pairs = [b.split(":") for b in line.split(":", 1)[1].split()]
                    hists[direction] = [[float(mid) / 1e3, int(count)] for mid, count in pairs]  # [us, count]
            return summary, hists
    return None, None


def profile_window(duration, label):
    """Open a broker measurement window for `duration` seconds while sampling /proc; return the result dict."""
    pid = broker_pid()
    if pid is None:
        raise RuntimeError(f"container {CONTAINER} is not running")
    if not os.access(f"/proc/{pid}/task", os.R_OK):
        raise RuntimeError(f"cannot read /proc/{pid} of the broker")

    subprocess.run(["docker", "kill", "-s", "USR1", CONTAINER], check=True, capture_output=True)
    t0 = time.monotonic()
    start = read_threads(pid)
    series = {"t": [], "cpu_pct": [], "rss_mb": []}
    rss_samples = []
    prev_ticks, prev_t = sum(v[1] for v in start.values()), t0
    while time.monotonic() - t0 < duration:
        time.sleep(SAMPLE_PERIOD_S)
        now = time.monotonic()
        ticks = sum(v[1] for v in read_threads(pid).values())
        rss_kb, _ = read_memory_kb(pid)
        series["t"].append(now - t0)
        series["cpu_pct"].append(100.0 * (ticks - prev_ticks) / CLK_TCK / (now - prev_t))
        series["rss_mb"].append(rss_kb / 1024)
        rss_samples.append(rss_kb / 1024)
        prev_ticks, prev_t = ticks, now
    end = read_threads(pid)
    window = time.monotonic() - t0
    _, hwm_kb = read_memory_kb(pid)
    subprocess.run(["docker", "kill", "-s", "USR2", CONTAINER], check=True, capture_output=True)

    summary = hists = None
    for _ in range(50):  # the broker prints the summary on its next loop pass
        summary, hists = parse_summary(broker_logs())
        if summary is not None and summary["window_s"] > 0.5 * window:
            break
        time.sleep(0.1)
    if summary is None:
        raise RuntimeError("the broker printed no summary (is it the profiling build?)")

    cpu, ctx = {}, {}
    for tid, (name, ticks, vol, nonvol) in end.items():
        group = thread_group(name)
        t_start = start.get(tid, (name, ticks, vol, nonvol))
        cpu[group] = cpu.get(group, 0.0) + 100.0 * (ticks - t_start[1]) / CLK_TCK / window
        c = ctx.setdefault(group, [0, 0])
        c[0] += (vol - t_start[2]) / window
        c[1] += (nonvol - t_start[3]) / window
    return {
        "label": label,
        "ues": int(summary["ues"]),
        "window_s": window,
        "broker": summary,
        "hist_us": hists,
        "cpu_pct": cpu,
        "cpu_total_pct": sum(cpu.values()),
        "ctx_switches_per_s": {g: {"voluntary": v[0], "involuntary": v[1]} for g, v in ctx.items()},
        "rss_mean_mb": sum(rss_samples) / len(rss_samples) if rss_samples else 0.0,
        "rss_max_mb": max(rss_samples) if rss_samples else 0.0,
        "rss_hwm_mb": hwm_kb / 1024,
        "series": series,
    }


# ---------------------------------------------------------------- testbed runs --------------------------------------


def run_with_testbed(n, args, out_dir):
    """Run run_multi_ue.sh with n UEs and profile the broker during its measurement window."""
    run_dir = os.path.join(out_dir, f"{n}ue")
    cmd = [RUN_MULTI_UE, "--ues", str(n), "--duration", str(args.duration), "--warmup", str(args.warmup),
           "--out", run_dir, *args.run_args]
    log(f"=== {n} UE(s): {' '.join(cmd)}")
    attached = threading.Event()
    with open(os.path.join(out_dir, f"{n}ue_testbed.log"), "w") as testbed_log:
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)

        def pump():  # keep draining the pipe so run_multi_ue.sh never blocks on output
            for line in proc.stdout:
                testbed_log.write(line)
                testbed_log.flush()
                if "All UEs attached" in line:
                    attached.set()

        reader = threading.Thread(target=pump, daemon=True)
        reader.start()
        while not attached.wait(1.0):
            if proc.poll() is not None:
                reader.join()
                log(f"{n} UE(s): the testbed exited before every UE attached; see {testbed_log.name}")
                return None
        log(f"{n} UE(s): all attached; warmup {args.warmup}s")
        time.sleep(args.warmup)
        # run_multi_ue.sh measures for --duration after the warmup, then stops the gNB: stay inside that window.
        window = max(1.0, args.duration - 2.0)
        log(f"{n} UE(s): profiling the broker for {window:.0f}s")
        try:
            result = profile_window(window, f"{n}ue")
        except RuntimeError as e:
            log(f"{n} UE(s): profiling failed: {e}")
            result = None
        proc.wait()
        reader.join()
    return result


# ---------------------------------------------------------------- output --------------------------------------------

CSV_FIELDS = ["label", "ues", "window_s", "gnb_dl_msps", "gnb_ul_msps", "ue_dl_msps", "ue_ul_msps", "slot_ms",
              "dl_n", "dl_mean_us", "dl_p50_us", "dl_p99_us", "dl_p999_us", "dl_max_us",
              "ul_n", "ul_mean_us", "ul_p50_us", "ul_p99_us", "ul_p999_us", "ul_max_us",
              "busy_pct", "cpu_main_pct", "cpu_zmq_io_pct", "cpu_total_pct",
              "ctx_involuntary_main_per_s", "rss_mean_mb", "rss_max_mb", "rss_hwm_mb"]


def csv_row(r):
    b = r["broker"]
    row = {"label": r["label"], "ues": r["ues"], "window_s": round(r["window_s"], 3)}
    for key in CSV_FIELDS:
        if key in b:
            row[key] = b[key]
    row["slot_ms"] = SLOT_SAMPLES / (b["gnb_dl_msps"] * 1e3) if b["gnb_dl_msps"] > 0 else ""
    row["cpu_main_pct"] = r["cpu_pct"].get("main", 0.0)
    row["cpu_zmq_io_pct"] = r["cpu_pct"].get("zmq_io", 0.0)
    row["cpu_total_pct"] = r["cpu_total_pct"]
    row["ctx_involuntary_main_per_s"] = r["ctx_switches_per_s"].get("main", {}).get("involuntary", 0.0)
    for key in ("rss_mean_mb", "rss_max_mb", "rss_hwm_mb"):
        row[key] = r[key]
    return {k: (round(v, 4) if isinstance(v, float) else v) for k, v in row.items()}


def print_table(results):
    print(f"\n{'run':>6} {'slot':>7} {'gNB DL':>8} {'UE DL':>8} {'DL service us':>22} {'UL service us':>22} "
          f"{'busy':>6} {'CPU main/io/total %':>20} {'RSS MB':>7}")
    print(f"{'':>6} {'ms':>7} {'Msps':>8} {'Msps':>8} {'p50/p99/max':>22} {'p50/p99/max':>22} {'%':>6}")
    for r in results:
        b, c = r["broker"], r["cpu_pct"]
        slot = SLOT_SAMPLES / (b["gnb_dl_msps"] * 1e3) if b["gnb_dl_msps"] > 0 else float("nan")
        dl = f"{b['dl_p50_us']:.1f}/{b['dl_p99_us']:.1f}/{b['dl_max_us']:.0f}"
        ul = f"{b['ul_p50_us']:.1f}/{b['ul_p99_us']:.1f}/{b['ul_max_us']:.0f}"
        cpu = f"{c.get('main', 0):.0f}/{c.get('zmq_io', 0):.0f}/{r['cpu_total_pct']:.0f}"
        print(f"{r['label']:>6} {slot:7.3f} {b['gnb_dl_msps']:8.2f} {b['ue_dl_msps']:8.2f} {dl:>22} {ul:>22} "
              f"{b['busy_pct']:6.1f} {cpu:>20} {r['rss_max_mb']:7.1f}")


def ccdf_from_hist(hist):
    import numpy as np

    if not hist:
        return np.array([]), np.array([])
    values = np.array([h[0] for h in hist])
    counts = np.array([h[1] for h in hist], dtype=float)
    order = np.argsort(values)
    values, counts = values[order], counts[order]
    # P(X >= value of each bucket)
    tail = counts[::-1].cumsum()[::-1] / counts.sum()
    return values, tail


DL_COLOR, UL_COLOR = "tab:blue", "tab:red"


def _ue_axis(ax, ues):
    ax.set_xlabel("UEs")
    ax.set_xticks(ues)
    ax.grid(True, alpha=0.3)


def plot(results, out_dir):
    """Main figure (key trends vs UE count) plus supplementary figures; returns the main figure's path."""
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    results = sorted(results, key=lambda r: r["ues"])
    ues = [r["ues"] for r in results]
    b = [r["broker"] for r in results]

    # Main figure: one message per panel, at most two lines each.
    fig, ((ax_lat, ax_thr), (ax_cpu, ax_mem)) = plt.subplots(2, 2, figsize=(11, 8))

    for key, label, color in (("dl", "DL", DL_COLOR), ("ul", "UL", UL_COLOR)):
        ax_lat.plot(ues, [x[f"{key}_p50_us"] for x in b], "-o", color=color, label=f"{label} p50")
        ax_lat.plot(ues, [x[f"{key}_p99_us"] for x in b], "--o", color=color, mfc="white", label=f"{label} p99")
    ax_lat.set(yscale="log", ylabel="Service time (us)", title="Broker service time")
    _ue_axis(ax_lat, ues)
    ax_lat.legend(ncol=2)

    ax_thr.plot(ues, [x["gnb_dl_msps"] for x in b], "-o", color="tab:green", label="gNB side (slot rate)")
    ax_thr.plot(ues, [x["ue_dl_msps"] for x in b], "-o", color="tab:purple", label="all UEs (aggregate)")
    ax_thr.axhline(SLOT_SAMPLES / (TARGET_SLOT_MS * 1e3), color="gray", linestyle=":", label="1 ms slot target")
    ax_thr.set(ylabel="Msps", title="Throughput")
    ax_thr.set_ylim(bottom=0)
    _ue_axis(ax_thr, ues)
    ax_thr.legend()

    ax_cpu.plot(ues, [r["cpu_total_pct"] for r in results], "-o", color="tab:orange")
    ax_cpu.axhline(100, color="gray", linestyle=":", label="one core")
    ax_cpu.set(ylabel="CPU % (all threads)", title="CPU usage")
    ax_cpu.set_ylim(0, 1.25 * max([100.0] + [r["cpu_total_pct"] for r in results]))
    _ue_axis(ax_cpu, ues)
    ax_cpu.legend()

    ax_mem.plot(ues, [r["rss_hwm_mb"] for r in results], "-o", color="tab:brown")
    ax_mem.set(ylabel="Peak RSS (MB)", title="Memory usage")
    ax_mem.set_ylim(bottom=0)
    _ue_axis(ax_mem, ues)

    fig.suptitle("zmq_broker profile vs number of UEs")
    fig.tight_layout()
    main_path = os.path.join(out_dir, "broker_profile.png")
    fig.savefig(main_path, dpi=130)
    plt.close(fig)

    # Supplementary: full service-time distributions.
    fig, axes = plt.subplots(1, 2, figsize=(12, 4.5), sharey=True)
    for ax, direction, title in ((axes[0], "dl", "DL service time"), (axes[1], "ul", "UL service time")):
        for r in results:
            x, y = ccdf_from_hist(r["hist_us"].get(direction))
            ax.step(x, y, where="post", label=r["label"])
        ax.set(xscale="log", yscale="log", xlabel="Service time (us)", title=title)
        ax.grid(True, which="both", alpha=0.3)
        ax.legend()
    axes[0].set_ylabel("CCDF  P(X >= x)")
    fig.tight_layout()
    fig.savefig(os.path.join(out_dir, "broker_service_ccdf.png"), dpi=130)
    plt.close(fig)

    # Supplementary: where the CPU goes and how often the broker is preempted.
    fig, (ax_thr_cpu, ax_ctx) = plt.subplots(1, 2, figsize=(12, 4.5))
    xs = list(range(len(results)))
    main = [r["cpu_pct"].get("main", 0.0) for r in results]
    io = [r["cpu_pct"].get("zmq_io", 0.0) for r in results]
    other = [max(0.0, r["cpu_total_pct"] - m - i) for r, m, i in zip(results, main, io)]
    ax_thr_cpu.bar(xs, main, label="main loop thread")
    ax_thr_cpu.bar(xs, io, bottom=main, label="ZMQ I/O thread")
    if max(other) >= 0.5:  # ZMQ's reaper thread etc.; normally idle
        ax_thr_cpu.bar(xs, other, bottom=[m + i for m, i in zip(main, io)], label="other")
    ax_thr_cpu.plot(xs, [x["busy_pct"] for x in b], "k-o", label="main loop busy (outside zmq_poll)")
    ax_thr_cpu.axhline(100, color="gray", linestyle=":")
    ax_thr_cpu.set_xticks(xs, [r["label"] for r in results])
    ax_thr_cpu.set_ylim(0, 1.3 * max([100.0] + [r["cpu_total_pct"] for r in results]))
    ax_thr_cpu.set(ylabel="CPU % (100 = one core)", title="CPU by thread")
    ax_thr_cpu.grid(True, axis="y", alpha=0.3)
    ax_thr_cpu.legend(fontsize=8, loc="upper left")
    for group, label in (("main", "main loop thread"), ("zmq_io", "ZMQ I/O thread")):
        ax_ctx.plot(ues, [r["ctx_switches_per_s"].get(group, {}).get("involuntary", 0.0) for r in results], "-o",
                    label=label)
    ax_ctx.set(ylabel="Involuntary context switches / s", title="Preemption of broker threads")
    _ue_axis(ax_ctx, ues)
    ax_ctx.legend(fontsize=8)
    fig.tight_layout()
    fig.savefig(os.path.join(out_dir, "broker_cpu_breakdown.png"), dpi=130)
    plt.close(fig)

    # Supplementary: CPU and memory over each measurement window.
    fig, (ax_cpu, ax_mem) = plt.subplots(1, 2, figsize=(12, 4.5))
    for r in results:
        ax_cpu.plot(r["series"]["t"], r["series"]["cpu_pct"], label=r["label"])
        ax_mem.plot(r["series"]["t"], r["series"]["rss_mb"], label=r["label"])
    ax_cpu.set(xlabel="Time in window (s)", ylabel="CPU % (all threads)", title="Broker CPU over the window")
    ax_mem.set(xlabel="Time in window (s)", ylabel="RSS (MB)", title="Broker memory over the window")
    ax_mem.set_ylim(bottom=0)
    for ax in (ax_cpu, ax_mem):
        ax.grid(True, alpha=0.3)
        ax.legend(fontsize=8)
    fig.tight_layout()
    fig.savefig(os.path.join(out_dir, "broker_timeseries.png"), dpi=130)
    plt.close(fig)
    return main_path


def save(results, out_dir):
    for r in results:
        with open(os.path.join(out_dir, f"{r['label']}.json"), "w") as f:
            json.dump(r, f, indent=1)
    with open(os.path.join(out_dir, "profile.csv"), "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=CSV_FIELDS)
        writer.writeheader()
        for r in sorted(results, key=lambda r: r["ues"]):
            writer.writerow(csv_row(r))


def load(out_dir):
    results = []
    for name in os.listdir(out_dir):
        if name.endswith("ue.json") or name == "attached.json":
            with open(os.path.join(out_dir, name)) as f:
                results.append(json.load(f))
    return results


# ---------------------------------------------------------------- main ----------------------------------------------


def main():
    argv = sys.argv[1:]
    run_args = []
    if "--" in argv:
        run_args = argv[argv.index("--") + 1:]
        argv = argv[:argv.index("--")]
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--ues", type=int, nargs="+", help="UE counts to run through run_multi_ue.sh")
    parser.add_argument("--duration", type=int, default=60, help="measured seconds per run (default 60)")
    parser.add_argument("--warmup", type=int, default=10, help="seconds after every UE attached (default 10)")
    parser.add_argument("--out", help="output directory")
    parser.add_argument("--replot", metavar="DIR", help="only redraw the plots from DIR/*.json")
    args = parser.parse_args(argv)
    args.run_args = run_args or DEFAULT_RUN_ARGS

    if args.replot:
        results = load(args.replot)
        print_table(sorted(results, key=lambda r: r["ues"]))
        print(f"\nSaved {plot(results, args.replot)}")
        return

    out_dir = os.path.abspath(args.out or os.path.join(TEST_DIR, "logs",
                                                         f"broker_profile_{datetime.datetime.now():%Y%m%d_%H%M%S}"))
    os.makedirs(out_dir, exist_ok=True)
    results = []
    if args.ues:
        for n in args.ues:
            r = run_with_testbed(n, args, out_dir)
            if r is not None:
                results.append(r)
                save(results, out_dir)
    else:
        log(f"Profiling the running broker for {args.duration}s")
        r = profile_window(args.duration, "attached")
        results.append(r)
        save(results, out_dir)
    if not results:
        log("No run produced a profile")
        sys.exit(1)
    print_table(sorted(results, key=lambda r: r["ues"]))
    log(f"Saved {plot(results, out_dir)} (+ broker_service_ccdf.png, broker_cpu_breakdown.png, broker_timeseries.png), "
        f"profile.csv and per-run JSON in {out_dir}")


if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal.default_int_handler)
    main()
