# Copyright (c) Microsoft Corporation. Licensed under the MIT license.
#
# dl_mcs_app - EdgeRIC-on-Janus DL MCS muApp (jrtc xApp).
#
# Report path (RX): subscribes to the dl_mcs codelet's per-UE per-grant telemetry
#   {rnti, du_ue_index, tti, cqi, mcs_table, cur_mcs}.
# Policy path (TX): computes a per-UE DL MCS and pushes it back into the codelet's
#   mcs_in control-input channel (concrete-device addressed). The codelet overrides
#   the link-adaptation MCS with this value for the UE's PDSCH grant. Each message
#   echoes the metrics `tti` for RANtime staleness gating in the codelet.
#
# Policy is read LIVE from POLICY_FILE (/tmp/edgeric_mcs_policy) each ~1s, so the
# operator can switch policy without redeploying:
#   passthrough        -> do NOT override (stock link adaptation)   [default]
#   fixed:N            -> force MCS N on all UEs
#   fixed:m0,m1[,...]  -> per-du_ue_index MCS
#   offset:K           -> cur_mcs + K per UE (K may be negative), clamped
# MCS is clamped to [0, MAX_MCS] (the codelet also clamps).

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

from dl_mcs_report import struct__dl_mcs_report

REPORT_SIDX = 0
CTRL_SIDX = 1

REPORT_URI = b"edgeric_mcs://jbpf_agent/edgeric_mcs/dl_mcs"
REPORT_CHAN = b"out_dl_mcs"
CTRL_URI = b"edgeric_mcs://jbpf_agent/edgeric_mcs/dl_mcs"
CTRL_CHAN = b"mcs_in"

POLICY_FILE = "/tmp/edgeric_mcs_policy"
DEFAULT_POLICY = os.environ.get("EDGERIC_MCS_POLICY", "passthrough")
LOG = "/tmp/edgeric_mcs.log"
MAX_MCS = int(os.environ.get("EDGERIC_MAX_MCS", "28"))   # 29..31 reserved for retx


def _clamp(m):
    m = int(m)
    if m < 0:
        m = 0
    if m > MAX_MCS:
        m = MAX_MCS
    return m


class St:
    def __init__(self):
        self.app = None
        self.fh = None
        self.ue = {}          # du_ue_index -> {cqi, cur_mcs, mcs_table, tti, rnti}
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


def _target_mcs(state):
    """Return {du_ue_index: mcs} to apply, or {} to leave link adaptation alone."""
    pol = state.policy
    if pol in ("passthrough", "off", ""):
        return {}
    if pol.startswith("fixed:"):
        vals = [x for x in pol.split(":", 1)[1].split(",")]
        out = {}
        for ue in state.ue:
            v = vals[ue] if ue < len(vals) else vals[-1]
            out[ue] = _clamp(v)
        return out
    if pol.startswith("offset:"):
        try:
            k = int(pol.split(":", 1)[1])
        except Exception:
            k = 0
        return {ue: _clamp(s["cur_mcs"] + k) for ue, s in state.ue.items()}
    return {}


def _send(state, ue, tti, mcs):
    # struct dl_mcs_msg { uint32 du_ue_index; uint32 tti; uint32 mcs; } (12 bytes)
    msg = struct.pack("<III", ue, tti, int(mcs))
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
    rep = ctypes.cast(data_entry.data, ctypes.POINTER(struct__dl_mcs_report)).contents
    ue = int(rep.du_ue_index)
    state.ue[ue] = {"cqi": int(rep.cqi), "cur_mcs": int(rep.cur_mcs),
                    "mcs_table": int(rep.mcs_table), "tti": int(rep.tti), "rnti": int(rep.rnti)}
    state.reports += 1

    now = time.time()
    _refresh_policy(state, now)

    tgt = _target_mcs(state)
    for u, mcs in tgt.items():
        _send(state, u, int(rep.tti), mcs)

    if now - state.last_log >= 1.0:
        state.last_log = now
        summ = " ".join(f"ue{u}(cqi={s['cqi']},cur_mcs={s['cur_mcs']}->{tgt.get(u,'-')})"
                        for u, s in sorted(state.ue.items()))
        line = f"[edgeric_mcs] policy={state.policy} reports={state.reports} sent={state.sent} failed={state.failed} | {summ}"
        print(line, flush=True)
        if state.fh:
            state.fh.write(line + "\n")
            state.fh.flush()


def jrtc_start_app(capsule):
    streams = [
        # RX: per-UE DL MCS telemetry (wildcard dest, like other telemetry streams)
        JrtcStreamCfg_t(
            JrtcStreamIdCfg_t(JRTC_ROUTER_REQ_DEST_ANY, JRTC_ROUTER_REQ_DEVICE_ID_ANY,
                              REPORT_URI, REPORT_CHAN),
            True, None),
        # TX: per-UE MCS control (CONCRETE device -- required for input channels)
        JrtcStreamCfg_t(
            JrtcStreamIdCfg_t(JRTC_ROUTER_REQ_DEST_NONE, 1,
                              CTRL_URI, CTRL_CHAN),
            False, None),
    ]

    app_cfg = JrtcAppCfg_t(
        b"dl_mcs_app",
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
        print(f"[edgeric_mcs] cannot open {LOG}: {e}", flush=True)
    print(f"[edgeric_mcs] starting, default_policy={DEFAULT_POLICY}", flush=True)

    state.app = jrtc_app_create(capsule, app_cfg, app_handler, state)
    jrtc_app_run(state.app)

    if state.fh:
        try:
            state.fh.flush(); state.fh.close()
        except Exception:
            pass
    print(f"[edgeric_mcs] exiting: reports={state.reports} sent={state.sent} failed={state.failed}", flush=True)
    jrtc_app_destroy(state.app)
