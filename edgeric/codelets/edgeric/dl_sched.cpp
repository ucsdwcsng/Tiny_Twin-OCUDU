// Copyright (c) Microsoft Corporation. Licensed under the MIT license.
//
// dl_sched - EdgeRIC-on-Janus DL scheduling telemetry + control, in ONE codelet.
//
// HOOK: mac_sched_dl_ctrl  (a CONTROL hook added to srsRAN; fires per-UE per-slot
// at the end of scheduler_time_pf::ue_ctxt::compute_dl_prio, before the DL queue
// is ordered). ctx.data -> jbpf_mac_sched_dl_ctrl_info.
//
// Report path : emit {rnti, du_ue_index, tti, cqi, dl_backlog_bytes, pf_prio} to
//               the muApp (throttled to every 4th slot to bound the rate).
// Policy path : drain per-UE weights the muApp pushed on ctrl_in, and OVERRIDE
//               the PF priority (dl_prio) with the muApp's weight -> the DL queue
//               is then ordered by the muApp decision (Max-Weight). Applied only
//               while fresh: RANtime staleness gate (ci.tti - weight.tti <= N),
//               exactly the EdgeRIC "apply only the latest, drop if stale" model.
//
// Priorities travel as IEEE754 float BITS (uint32) so this eBPF codelet never
// does floating point -- it just copies the muApp's 4 bytes into new_prio_bits.

#include <linux/bpf.h>
#include <string.h>

#include "jbpf_srsran_contexts.h"
#include "dl_sched_report.pb.h"

#include "../utils/misc_utils.h"

#define SEC(NAME) __attribute__((section(NAME), used))

#include "jbpf_defs.h"
#include "jbpf_helper.h"

#define MAX_UE      (16)
#define STALE_TTIS  (200)   // honor a muApp decision within 200 slots (~200ms) of its metrics
                            // tti -> tolerant of the xApp round-trip; expires if the muApp dies
#define EMIT_MASK   (0x3)   // emit telemetry every 4th slot (rate control)

// Layout MUST MATCH struct jbpf_mac_sched_dl_ctrl_info in the gNB's
// include/srsran/jbpf/jbpf_srsran_contexts.h (codelets build against the SDK
// image headers, not the modified gNB tree; this is the wire contract).
struct jbpf_mac_sched_dl_ctrl_info {
    uint16_t rnti;
    uint16_t du_ue_index;
    uint32_t tti;
    uint32_t cqi;
    uint32_t dl_backlog_bytes;
    uint32_t cur_max_rbs;      // srsRAN -> codelet: fair per-UE RB cap this slot
    uint32_t new_rbs_valid;    // codelet -> srsRAN: nonzero = apply new_max_rbs
    uint32_t new_max_rbs;      // codelet -> srsRAN: per-UE RB-share cap
};

// muApp -> codelet: per-UE DL RB-share cap (integer PRBs), stamped with the
// metrics tti it reacted to (RANtime echo for staleness).
struct dl_weight_msg {
    uint32_t du_ue_index;
    uint32_t tti;
    uint32_t max_rbs;
};

jbpf_ringbuf_map(out_dl_sched, dl_sched_report, 16);
jbpf_control_input_map(weight_in, dl_weight_msg, 32);

struct weight_ent {
    uint32_t max_rbs;
    uint32_t tti;
};
struct jbpf_load_map_def SEC("maps") weight_map = {
    .type        = JBPF_MAP_TYPE_ARRAY,
    .key_size    = sizeof(uint32_t),
    .value_size  = sizeof(struct weight_ent),
    .max_entries = MAX_UE,
};

// Per-codelet call counter for a robust emit throttle that does NOT depend on the
// gNB RANtime counter (which is used only for staleness). Emit every EMIT_EVERY calls.
#define EMIT_EVERY (16)
struct jbpf_load_map_def SEC("maps") emit_ctr = {
    .type        = JBPF_MAP_TYPE_ARRAY,
    .key_size    = sizeof(uint32_t),
    .value_size  = sizeof(uint32_t),
    .max_entries = 1,
};

extern "C" SEC("jbpf_srsran_generic")
uint64_t jbpf_main(void* state)
{
    struct jbpf_ran_generic_ctx* ctx = (jbpf_ran_generic_ctx*)state;

    struct jbpf_mac_sched_dl_ctrl_info* ci = (struct jbpf_mac_sched_dl_ctrl_info*)ctx->data;
    if (reinterpret_cast<uint8_t*>(ci) + sizeof(struct jbpf_mac_sched_dl_ctrl_info) >
        reinterpret_cast<uint8_t*>(ctx->data_end)) {
        return JBPF_CODELET_FAILURE;
    }

    // 1. Report: emit this UE's state to the muApp (rate-limited via a call counter).
    uint32_t czero = 0;
    uint32_t* cnt = (uint32_t*)jbpf_map_lookup_elem(&emit_ctr, &czero);
    if (cnt) {
        *cnt = *cnt + 1;
    }
    if (cnt && (*cnt % EMIT_EVERY) == 0) {
        dl_sched_report rep;
        memset(&rep, 0, sizeof(rep));
        rep.rnti             = ci->rnti;
        rep.du_ue_index      = ci->du_ue_index;
        rep.tti              = ci->tti;
        rep.cqi              = ci->cqi;
        rep.dl_backlog_bytes = ci->dl_backlog_bytes;
        rep.cur_max_rbs      = ci->cur_max_rbs;
        jbpf_ringbuf_output(&out_dl_sched, (void*)&rep, sizeof(rep));
    }

    // 2. Policy: drain any muApp weight messages into weight_map.
    struct dl_weight_msg m;
    #pragma unroll
    for (int n = 0; n < 8; n++) {
        int got = jbpf_control_input_receive(&weight_in, &m, sizeof(m));
        if (got <= 0) {
            break;
        }
        uint32_t k = m.du_ue_index % MAX_UE;
        struct weight_ent e;
        e.max_rbs = m.max_rbs;
        e.tti     = m.tti;
        jbpf_map_update_elem(&weight_map, &k, &e, 0);
    }

    // 3. Apply the freshest muApp RB-share cap for THIS UE (staleness-gated).
    uint32_t key = (uint32_t)ci->du_ue_index % MAX_UE;
    struct weight_ent* e = (struct weight_ent*)jbpf_map_lookup_elem(&weight_map, &key);
    if (e) {
        uint32_t age = ci->tti - e->tti;   // unsigned RANtime staleness
        if (age <= STALE_TTIS) {
            ci->new_max_rbs   = e->max_rbs;
            ci->new_rbs_valid = 1;
        }
    }

    return JBPF_CODELET_SUCCESS;
}
