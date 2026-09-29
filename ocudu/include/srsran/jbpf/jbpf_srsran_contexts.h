#ifndef JBPF_SRSRAN_CONTEXTS_H
#define JBPF_SRSRAN_CONTEXTS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>


#include "jbpf_helper_api_defs_ext.h"

typedef enum {
    JBPF_PROG_TYPE_RAN_OFH = CUSTOM_PROGRAM_START_ID,
    JBPF_PROG_TYPE_RAN_LAYER2,
    JBPF_PROG_TYPE_RAN_MAC_SCHED,
    JBPF_PROG_TYPE_RAN_GENERIC    
} jbpf_srsran_index_e;

typedef uint8_t JbpfDirection_t;
#define JBPF_DL (0)
#define JBPF_UL (1)

/* OFH context*/
struct jbpf_ran_ofh_ctx {
  uint64_t data; /* Pointer to beginning of buffer with int16_t IQ samples */
  uint64_t data_end; /* Pointer to end+1 of packet */
  uint64_t meta_data; /* Used by ebpf */
  /* Combination of frame, slot and cell_id, provide a unique
     context for an execution pipeline */
  uint16_t ctx_id; /* Context id (could be implementation specific) */
  JbpfDirection_t direction; /* 0 DL, 1 UL */
};

/* L2 context*/
struct jbpf_ran_layer2_ctx {
    uint64_t data; /* Pointer to beginning of variable-sized L2 struct */
    uint64_t data_end; /* Pointer to end+1 of variable-sized struct */
    uint64_t meta_data; /* Used for the program to store metadata */
    uint16_t ctx_id; 
    uint16_t frame; /* 3GPP frame number */
    uint16_t slot; /* 3GPP slot number */
    uint16_t cell_id; /* Cell id */
};


/* DU UE context */
struct jbpf_du_ue_ctx_info {
    uint16_t ctx_id;   /* Context id (could be implementation specific) */
    uint32_t du_ue_index;
    uint32_t tac;
    uint32_t plmn;
    uint64_t nci;
    uint16_t pci;
    uint16_t crnti;
};


/* MAC HARQ context */

typedef uint8_t JbpHarqEvent_t;
#define JBPF_HARQ_EVENT_TX (0)
#define JBPF_HARQ_EVENT_RETX (1)
#define JBPF_HARQ_EVENT_FAILURE (2)
#define JBPF_HARQ_EVENT_NUM (3)

struct jbpf_mac_sched_harq_ctx_info {
    JbpHarqEvent_t  harq_type; /* 0=TX, 1=RETX, 2=HARQ_FAILURE */
    uint8_t         harq_id;
    bool            ndi;
    uint8_t         nof_retxs;
    uint8_t         max_nof_harq_retxs;
    uint8_t         mcs_table;
    uint8_t         mcs;
    uint32_t        tbs_bytes;
    bool            slice_id_present;
    uint8_t         slice_id; 
};

struct jbpf_mac_sched_harq_ctx_info_dl {
    struct jbpf_mac_sched_harq_ctx_info h;
    uint8_t                             cqi;
};


/* MAC Scheduler context */
struct jbpf_mac_sched_ctx {
    uint64_t data; /* Pointer to beginning of variable-sized L2 struct */
    uint64_t data_end; /* Pointer to end+1 of variable-sized struct */
    uint64_t meta_data; /* Used by ebpf */
    uint64_t srs_meta_data1; /* Used for the program to store metadata */
    uint16_t ctx_id; /* Context id (could be implementation specific) */
    uint16_t du_ue_index; /* UE index */
    uint16_t cell_id; /* Cell id */
    uint16_t rnti; /* UE RNTI */
};


/* MAC scheduler DL control (EdgeRIC-style Max-Weight via PRB-share).
 * Fires per-UE per-slot at the DL grant point (scheduler_time_rr::alloc_dl_ue_newtx),
 * which is scheduler-policy-agnostic and works under the round-robin scheduler
 * (the PF scheduler in this fork collapses with >=2 UEs). srsRAN fills the
 * telemetry (rnti..cur_max_rbs) and passes a pointer to this struct as ctx.data;
 * a control codelet may WRITE new_max_rbs (+ set new_rbs_valid) to OVERRIDE the
 * per-UE maximum RB allocation for this slot -- i.e. the muApp turns a per-UE
 * weight into a share of the cell's PRBs (EdgeRIC's weight->PRB mechanism).
 * All fields are plain integers so the eBPF codelet needs no floating point. */
struct jbpf_mac_sched_dl_ctrl_info {
    uint16_t rnti;             /* srsRAN -> codelet */
    uint16_t du_ue_index;      /* srsRAN -> codelet */
    uint32_t tti;              /* srsRAN -> codelet: RANtime slot counter */
    uint32_t cqi;              /* srsRAN -> codelet: wideband CQI */
    uint32_t dl_backlog_bytes; /* srsRAN -> codelet: pending DL newTx bytes (backlog) */
    uint32_t cur_max_rbs;      /* srsRAN -> codelet: fair per-UE RB cap this slot (reference) */
    uint32_t new_rbs_valid;    /* codelet -> srsRAN: nonzero = apply new_max_rbs */
    uint32_t new_max_rbs;      /* codelet -> srsRAN: per-UE RB-share cap (weight->PRBs) */
};

/* CONTROL context for the mac_sched_dl_mcs_ctrl hook: per-UE DL MCS override at
 * the PDSCH MCS-selection (link-adaptation) point. All integers (no FP). */
struct jbpf_mac_sched_dl_mcs_ctrl_info {
    uint16_t rnti;             /* srsRAN -> codelet */
    uint16_t du_ue_index;      /* srsRAN -> codelet */
    uint32_t tti;              /* srsRAN -> codelet: RANtime slot counter */
    uint32_t cqi;              /* srsRAN -> codelet: wideband CQI */
    uint32_t mcs_table;        /* srsRAN -> codelet: PDSCH MCS table (0 qam64, 1 qam256, 2 qam64LowSe) */
    uint32_t cur_mcs;          /* srsRAN -> codelet: link-adaptation chosen DL MCS (reference) */
    uint32_t new_mcs_valid;    /* codelet -> srsRAN: nonzero = apply new_mcs */
    uint32_t new_mcs;          /* codelet -> srsRAN: per-UE DL MCS override (0..31) */
};


/* E1 context info */
struct jbpf_cucp_e1_ctx_info {
    uint16_t ctx_id;   /* Context id (could be implementation specific) */
    uint64_t cu_cp_ue_index; 
    uint64_t gnb_cu_cp_ue_e1ap_id; 
    uint64_t gnb_cu_up_ue_e1ap_id; 
};
struct jbpf_cuup_e1_ctx_info {
    uint16_t ctx_id;   /* Context id (could be implementation specific) */
    uint64_t cu_up_ue_index; 
    uint64_t gnb_cu_cp_ue_e1ap_id; 
    uint64_t gnb_cu_up_ue_e1ap_id; 
};

/* CUCP UE context info */
struct jbpf_cucp_uemgr_ctx_info {
    uint16_t ctx_id;    /* Context id (could be implementation specific) */
    uint16_t du_index;  
    uint32_t plmn;      /* (mcc << 16) || mnc */ 
    uint64_t cu_cp_ue_index; 
};

/* RRC context info */
struct jbpf_rrc_ctx_info {
    uint16_t ctx_id;    /* Context id (could be implementation specific) */
    uint64_t cu_cp_ue_index; 
};

typedef enum {
    RRC_SETUP = 1,
    RRC_RECONFIGURATION,
    RRC_REESTABLISHMENT,
    RRC_UE_CAPABILITY,
    RRC_PROCEDURE_MAX
} JbpfRrcProcedure_t;

/* RLC context info */

typedef enum {
    JBPF_RLC_MODE_TM = 1,
    JBPF_RLC_MODE_UM,
    JBPF_RLC_MODE_AM,
    JBPF_RLC_MODE_MAX
} JbpfRlcMode_t;

typedef enum {
    JBPF_RLC_PDUTYPE_STATUS = 1,
    JBPF_RLC_PDUTYPE_DATA,
    JBPF_RLC_PDUTYPE_DATA_RETX,
    JBPF_RLC_PDUTYPE_MAX
} JbpfRlcPdu_t;


typedef struct {
    uint8_t used;      /* Is the window used, 0 = not-used, 1 = used */
    uint32_t num_pkts;     /* Total packets */
    uint32_t num_bytes;    /* Total bytes*/
} jbpf_queue_info_t;

struct jbpf_rlc_ctx_info {
    uint16_t ctx_id;    /* Context id (could be implementation specific) */
    uint64_t gnb_du_id;
    uint16_t du_ue_index; 
    uint8_t is_srb;  /* true=srb, false=drb */
    uint8_t rb_id;   /* if is_srb=True:    0=srb0, 1=srb1, 2=srb2,
                     if is_srb=False:      1=drb1, 2=drb2, 3-drb3 ... */
    JbpfDirection_t direction; /* 0 DL, 1 UL */
    JbpfRlcMode_t rlc_mode;  /* 0=TM, 1=UM, 2=AM*/

    union {
        struct {
            jbpf_queue_info_t sdu_queue_info; /* SDU queue info */
        } tm_tx;
        struct {
            jbpf_queue_info_t sdu_queue_info; /* SDU queue info */
        } um_tx;
        struct {
            jbpf_queue_info_t sdu_queue_info; /* SDU queue info */
            jbpf_queue_info_t window_info;  /* Window info */
        } am_tx;
        struct {
            uint32_t window_num_pkts;  /* Window info */
        } um_rx;
        struct {
            uint32_t window_num_pkts;  /* Window info */
        } am_rx;
    } u;
};


/* PDCP context */

struct jbpf_pdcp_ctx_info {
    uint16_t ctx_id;   /* Context id (could be implementation specific) */
    uint32_t cu_ue_index;   /* if is_srb=True is cu_cp_ue_index, if is_srb=False is cu_up_ue_index */
    uint8_t is_srb; /* true=srb, false=drb */
    uint8_t rb_id;   /* if is_srb=True:    0=srb0, 1=srb1, 2=srb2,
                        if is_srb=False:   1=drb1, 2=drb2, 3-drb3 ... */
    uint8_t rlc_mode;  /* 0=UM, 1=AM*/

    // window details
    jbpf_queue_info_t window_info;  /* Window info */
};  

typedef enum {
    NGAP_PROCEDURE_INITIAL_CONTEXT_SETUP = 1,
    NGAP_PROCEDURE_UE_CONTEXT_RELEASE,
    NGAP_PROCEDURE_PDU_SESSION_SETUP,
    NGAP_PROCEDURE_PDU_SESSION_MODIFY,
    NGAP_PROCEDURE_PDU_SESSION_RELEASE,
    NGAP_PROCEDURE_RESOURCE_ALLOCATION,
    NGAP_PROCEDURE_MAX
} JbpfNgapProcedure_t;


/* NBAP context */

struct jbpf_ngap_ctx_info {
    uint16_t ctx_id;    /* Context id (could be implementation specific) */
    uint64_t cucp_ue_index; 
    uint16_t ran_ue_ngap_id_set;
    uint64_t ran_ue_ngap_id; /* RAN UE NGAP ID */
    uint16_t amf_ue_ngap_id_set;
    uint64_t amf_ue_ngap_id; /* AMF UE NGAP ID */
};

/* srsRAN generic context */
struct jbpf_ran_generic_ctx {
    uint64_t data; /* Pointer to beginning of buffer with int16_t IQ samples */
    uint64_t data_end; /* Pointer to end+1 of packet */
    uint64_t meta_data; /* Used by ebpf */
    uint64_t srs_meta_data1; /* Used for the program to store metadata */
    uint64_t srs_meta_data2; /* Used for the program to store metadata */
    uint64_t srs_meta_data3; /* Used for the program to store metadata */
    uint64_t srs_meta_data4; /* Used for the program to store metadata */
};

/* CONTROL context for the rlc_dl_ctrl hook (RLC AM DRBs only; not called for SRBs).
 * srsRAN fills the identity + cur_* limits and passes a pointer to this struct
 * as ctx.data; the control codelet may WRITE new_byte_limit / new_sdu_limit
 * through ctx.data, and srsRAN reads them back after the hook returns. 0 means
 * "no change"; other values are clamped by the gNB (byte limit to
 * [one max PDCP PDU, configured queue-bytes], SDU limit to [1, configured queue-size]).
 * This is the actuation path for dynamic RLC buffer management. */
struct jbpf_rlc_ctrl_info {
    uint16_t du_ue_index;
    uint8_t  is_srb;   /* always 0 (kept for layout compatibility) */
    uint8_t  rb_id;    /* DRB id */
    uint32_t cur_byte_limit;   /* srsRAN -> codelet: the queue's current byte limit */
    uint32_t new_byte_limit;   /* codelet -> srsRAN: desired byte limit (0 = leave unchanged) */
    uint32_t cur_sdu_limit;    /* srsRAN -> codelet: the queue's current SDU-count limit */
    uint32_t new_sdu_limit;    /* codelet -> srsRAN: desired SDU-count limit (0 = leave unchanged) */
    uint32_t cfg_byte_limit;   /* srsRAN -> codelet: CONFIGURED byte limit (queue-bytes); write it back to reset */
    uint32_t cfg_sdu_limit;    /* srsRAN -> codelet: CONFIGURED SDU limit (queue-size = queue capacity) */
};
  




#ifdef __cplusplus
}
#endif

#endif // JBPF_SRSRAN_CONTEXTS_H
