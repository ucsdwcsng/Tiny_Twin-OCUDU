// Copyright (c) Microsoft Corporation. Licensed under the MIT license.
//
// dl_mcs - EdgeRIC-on-Janus DL MCS telemetry + control, in ONE codelet.
//
// HOOK: mac_sched_dl_mcs_ctrl (a CONTROL hook in the OCUDU gNB; fires per newTx UE
// at the PDSCH MCS-selection point in grant_params_selector, right after link
// adaptation picks the DL MCS). ctx.data -> jbpf_mac_sched_dl_mcs_ctrl_info.
//
// Report path : emit {rnti, du_ue_index, tti, cqi, mcs_table, cur_mcs} to the xApp
//               (throttled to every EMIT_EVERY calls to bound the rate).
// Policy path : drain per-UE MCS the xApp pushed on mcs_in, and OVERRIDE the
//               link-adaptation MCS (new_mcs) for this UE's grant. Applied only
//               while fresh: RANtime staleness gate (ci.tti - msg.tti <= N),
//               the EdgeRIC "apply only the latest, drop if stale" model.
//
// MCS values are plain integers (0..31) so this eBPF codelet needs no floating point.

#include <linux/bpf.h>
#include <string.h>

#include "jbpf_srsran_contexts.h"
#include "dl_mcs_report.pb.h"

#include "../utils/misc_utils.h"

#define SEC(NAME) __attribute__((section(NAME), used))

#include "jbpf_defs.h"
#include "jbpf_helper.h"

#define MAX_UE      (16)
#define STALE_TTIS  (200)   // honor an xApp decision within ~200 slots of its metrics tti
#define MAX_MCS     (28)    // clamp override to a data MCS (29..31 are reserved for retx)

// Layout MUST MATCH struct jbpf_mac_sched_dl_mcs_ctrl_info in the gNB's
// include/srsran/jbpf/jbpf_srsran_contexts.h (codelets build against the SDK image
// headers, not the modified gNB tree; this is the wire contract).
struct jbpf_mac_sched_dl_mcs_ctrl_info {
    uint16_t rnti;
    uint16_t du_ue_index;
    uint32_t tti;
    uint32_t cqi;
    uint32_t mcs_table;      // srsRAN -> codelet: 0 qam64, 1 qam256, 2 qam64LowSe
    uint32_t cur_mcs;        // srsRAN -> codelet: link-adaptation chosen DL MCS
    uint32_t new_mcs_valid;  // codelet -> srsRAN: nonzero = apply new_mcs
    uint32_t new_mcs;        // codelet -> srsRAN: per-UE DL MCS override (0..31)
};

// xApp -> codelet: per-UE DL MCS, stamped with the metrics tti it reacted to.
struct dl_mcs_msg {
    uint32_t du_ue_index;
    uint32_t tti;
    uint32_t mcs;
};

jbpf_ringbuf_map(out_dl_mcs, dl_mcs_report, 16);
jbpf_control_input_map(mcs_in, dl_mcs_msg, 32);

struct mcs_ent {
    uint32_t mcs;
    uint32_t tti;
};
struct jbpf_load_map_def SEC("maps") mcs_map = {
    .type        = JBPF_MAP_TYPE_ARRAY,
    .key_size    = sizeof(uint32_t),
    .value_size  = sizeof(struct mcs_ent),
    .max_entries = MAX_UE,
};

// Per-codelet call counter for a robust emit throttle (independent of the gNB
// RANtime counter, which is used only for staleness). Emit every EMIT_EVERY calls.
#define EMIT_EVERY (16)
struct jbpf_load_map_def SEC("maps") mcs_emit_ctr = {
    .type        = JBPF_MAP_TYPE_ARRAY,
    .key_size    = sizeof(uint32_t),
    .value_size  = sizeof(uint32_t),
    .max_entries = 1,
};

extern "C" SEC("jbpf_srsran_generic")
uint64_t jbpf_main(void* state)
{
    struct jbpf_ran_generic_ctx* ctx = (jbpf_ran_generic_ctx*)state;

    struct jbpf_mac_sched_dl_mcs_ctrl_info* ci = (struct jbpf_mac_sched_dl_mcs_ctrl_info*)ctx->data;
    if (reinterpret_cast<uint8_t*>(ci) + sizeof(struct jbpf_mac_sched_dl_mcs_ctrl_info) >
        reinterpret_cast<uint8_t*>(ctx->data_end)) {
        return JBPF_CODELET_FAILURE;
    }

    // 1. Report: emit this UE's state to the xApp (rate-limited via a call counter).
    uint32_t czero = 0;
    uint32_t* cnt = (uint32_t*)jbpf_map_lookup_elem(&mcs_emit_ctr, &czero);
    if (cnt) {
        *cnt = *cnt + 1;
    }
    if (cnt && (*cnt % EMIT_EVERY) == 0) {
        dl_mcs_report rep;
        memset(&rep, 0, sizeof(rep));
        rep.rnti        = ci->rnti;
        rep.du_ue_index = ci->du_ue_index;
        rep.tti         = ci->tti;
        rep.cqi         = ci->cqi;
        rep.mcs_table   = ci->mcs_table;
        rep.cur_mcs     = ci->cur_mcs;
        jbpf_ringbuf_output(&out_dl_mcs, (void*)&rep, sizeof(rep));
    }

    // 2. Policy: drain any xApp MCS messages into mcs_map.
    struct dl_mcs_msg m;
    #pragma unroll
    for (int n = 0; n < 8; n++) {
        int got = jbpf_control_input_receive(&mcs_in, &m, sizeof(m));
        if (got <= 0) {
            break;
        }
        uint32_t k = m.du_ue_index % MAX_UE;
        struct mcs_ent e;
        e.mcs = m.mcs;
        e.tti = m.tti;
        jbpf_map_update_elem(&mcs_map, &k, &e, 0);
    }

    // 3. Apply the freshest xApp MCS for THIS UE (staleness-gated, clamped).
    uint32_t key = (uint32_t)ci->du_ue_index % MAX_UE;
    struct mcs_ent* e = (struct mcs_ent*)jbpf_map_lookup_elem(&mcs_map, &key);
    if (e) {
        uint32_t age = ci->tti - e->tti;   // unsigned RANtime staleness
        if (age <= STALE_TTIS) {
            uint32_t mcs = e->mcs;
            if (mcs > MAX_MCS) {
                mcs = MAX_MCS;
            }
            ci->new_mcs       = mcs;
            ci->new_mcs_valid = 1;
        }
    }

    return JBPF_CODELET_SUCCESS;
}
