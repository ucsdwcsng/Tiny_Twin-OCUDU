# Copyright (c) Microsoft Corporation. Licensed under the MIT license.
#
# dl_sched_app - EdgeRIC-on-Janus DL scheduling muApp (jrtc xApp).
#
# Report path (RX): subscribes to the dl_sched codelet's per-UE per-slot telemetry
#   {rnti, du_ue_index, tti, cqi, dl_backlog_bytes, pf_prio}.
# Policy path (TX): computes a per-UE DL scheduling WEIGHT and pushes it back into
#   the codelet's weight_in control-input channel (concrete-device addressed). The
#   codelet overrides the PF dl_prio with this weight, so the DL queue is ordered
#   by the muApp decision. Each weight echoes the metrics `tti` for RANtime
#   staleness gating in the codelet (EdgeRIC "apply latest, drop if stale").
#
# Policies (EDGERIC_POLICY env):
#   maxweight            -> weight_i = cqi_i * backlog_i   (EdgeRIC Max-Weight; default)
#   fixed:w0,w1[,...]    -> static per-du_ue_index weights (for a stark control demo)

import os
import sys
import time
import struct
import ctypes

JRTC_APP_PATH = os.environ.get("JRTC_APP_PATH")
if JRTC_APP_PATH is None:
    raise ValueError("JRTC_APP_PATH not set")
sys.path.append(f"{JRTC_APP_PATH}")

import jrtc_app
from jrtc_app import *

from dl_sched_report import struct__dl_sched_report

REPORT_SIDX = 0
CTRL_SIDX = 1

REPORT_URI = b"edgeric_sched://jbpf_agent/edgeric_sched/dl_sched"
REPORT_CHAN = b"out_dl_sched"
CTRL_URI = b"edgeric_sched://jbpf_agent/edgeric_sched/dl_sched"
CTRL_CHAN = b"weight_in"

# Policy is read LIVE from POLICY_FILE each ~1s (falls back to env, then maxweight),
# so the operator can switch policy without redeploying:
#   echo "fixed:100,1" > /tmp/edgeric_policy   # bias UE0 (stark control demo)
#   echo "fixed:1,100" > /tmp/edgeric_policy   # bias UE1
#   echo "maxweight"   > /tmp/edgeric_policy   # EdgeRIC Max-Weight (cqi*backlog)
POLICY_FILE = "/tmp/edgeric_policy"
DEFAULT_POLICY = os.environ.get("EDGERIC_POLICY", "maxweight")
LOG = "/tmp/edgeric_sched.log"

# Total DL PRBs of the cell (20 MHz / 15 kHz SCS = 106). The muApp turns per-UE
# weights into an integer RB-share cap = round(fraction * TOTAL_RBS), which the
# codelet applies as the UE's max RB allocation for the slot (EdgeRIC weight->PRBs).
TOTAL_RBS = int(os.environ.get("EDGERIC_TOTAL_RBS", "106"))
MIN_RBS = 1   # floor so a UE keeps a sliver rather than fully stalling its RLC


class St:
    def __init__(self):
        self.app = None
        self.fh = None
        self.ue = {}          # du_ue_index -> {cqi, backlog, tti, rnti}
        self.sent = 0
        self.failed = 0
        self.reports = 0
        self.last_log = 0.0
        self.policy = DEFAULT_POLICY
        self.last_pol = 0.0


def _refresh_policy(state, now):
    if now - state.last_pol < 1.0:
        return
    state.last_pol = now
    try:
        with open(POLICY_FILE) as f:
            p = f.read().strip()
        if p:
            state.policy = p
    except Exception:
        state.policy = DEFAULT_POLICY


def _weights(state):
    pol = state.policy
    if pol.startswith("fixed:"):
        vals = [float(x) for x in pol.split(":", 1)[1].split(",")]
        return {ue: (vals[ue] if ue < len(vals) else 1.0) for ue in state.ue}
    # Max-Weight: cqi * backlog. +1 on each factor so an idle-but-present UE keeps
    # a tiny nonzero weight rather than being fully starved.
    return {ue: (float(s["cqi"]) + 1.0) * (float(s["backlog"]) + 1.0)
            for ue, s in state.ue.items()}


def _rb_caps(state):
    """Turn per-UE weights into integer RB-share caps summing to ~TOTAL_RBS."""
    w = _weights(state)
    tot = sum(w.values())
    if tot <= 0:
        return {ue: TOTAL_RBS for ue in w}
    caps = {}
    for ue, wu in w.items():
        caps[ue] = max(MIN_RBS, int(round((wu / tot) * TOTAL_RBS)))
    return caps


def _send(state, ue, tti, max_rbs):
    # struct dl_weight_msg { uint32 du_ue_index; uint32 tti; uint32 max_rbs; } (12 bytes)
    msg = struct.pack("<III", ue, tti, int(max_rbs))
    res = jrtc_app_router_channel_send_input_msg(state.app, CTRL_SIDX, msg, len(msg))
    if res == 0:
        state.sent += 1
    else:
        state.failed += 1
    return res


def app_handler(timeout, stream_idx, data_entry, state):
    if timeout:
        return
    if stream_idx != REPORT_SIDX:
        return
    rep = ctypes.cast(data_entry.data, ctypes.POINTER(struct__dl_sched_report)).contents
    ue = int(rep.du_ue_index)
    state.ue[ue] = {"cqi": int(rep.cqi), "backlog": int(rep.dl_backlog_bytes),
                    "tti": int(rep.tti), "rnti": int(rep.rnti)}
    state.reports += 1

    now = time.time()
    _refresh_policy(state, now)

    caps = _rb_caps(state)
    for u, rbs in caps.items():
        _send(state, u, int(rep.tti), rbs)

    if now - state.last_log >= 1.0:
        state.last_log = now
        summ = " ".join(f"ue{u}(cqi={s['cqi']},bl={s['backlog']},rbs={caps.get(u,0)}/{TOTAL_RBS})"
                        for u, s in sorted(state.ue.items()))
        line = f"[edgeric_sched] policy={state.policy} reports={state.reports} sent={state.sent} failed={state.failed} | {summ}"
        print(line, flush=True)
        if state.fh:
            state.fh.write(line + "\n")
            state.fh.flush()


def jrtc_start_app(capsule):
    streams = [
        # RX: per-UE DL telemetry (wildcard dest, like other telemetry streams)
        JrtcStreamCfg_t(
            JrtcStreamIdCfg_t(JRTC_ROUTER_REQ_DEST_ANY, JRTC_ROUTER_REQ_DEVICE_ID_ANY,
                              REPORT_URI, REPORT_CHAN),
            True, None),
        # TX: per-UE weight control (CONCRETE device -- required for input channels)
        JrtcStreamCfg_t(
            JrtcStreamIdCfg_t(JRTC_ROUTER_REQ_DEST_NONE, 1,
                              CTRL_URI, CTRL_CHAN),
            False, None),
    ]

    app_cfg = JrtcAppCfg_t(
        b"dl_sched_app",
        100,
        len(streams),
        (JrtcStreamCfg_t * len(streams))(*streams),
        10.0,     # initialization_timeout_secs
        0.001,    # sleep_timeout_secs (Tiny-Twin: was 0.5; poll ~1 ms so reports are consumed before they go stale)
        3600.0,   # inactivity_timeout_secs
    )

    state = St()
    try:
        state.fh = open(LOG, "a")
    except Exception as e:
        print(f"[edgeric_sched] cannot open {LOG}: {e}", flush=True)
    print(f"[edgeric_sched] starting, default_policy={DEFAULT_POLICY}", flush=True)

    state.app = jrtc_app_create(capsule, app_cfg, app_handler, state)
    jrtc_app_run(state.app)

    if state.fh:
        try:
            state.fh.flush(); state.fh.close()
        except Exception:
            pass
    print(f"[edgeric_sched] exiting: reports={state.reports} sent={state.sent} failed={state.failed}", flush=True)
    jrtc_app_destroy(state.app)
