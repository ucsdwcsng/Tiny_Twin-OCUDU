#ifndef JBPF_SRSRAN_HOOKS_
#define JBPF_SRSRAN_HOOKS_

/*
 * UE Indexing Across Subsystems
 *
 * This comment outlines how UE (User Equipment) indexes are managed and tracked across 
 * various subsystems (DU, CU-CP, and CU-UP).
 *
 * All functions invoked as hooks for UE-specific operations will include a "UE index".
 * However, the DU, CU-CP, and CU-UP subsystems manage their UE indexes independently.
 * As a result, the same UE may have different index values in each subsystem.
 *
 * To clarify this in the code, the context structures use the following field names:
 *     - du_ue_index
 *     - cu_cp_ue_index
 *     - cu_up_ue_index
 *
 * Mapping UE indexes across subsystems involves the following conventions:
 *
 * DU (Distributed Unit):
 * -----------------------
 * The function `du_ue_ctx_creation` is called when a UE is created at the DU.
 * Parameters:
 *     - du_ue_index
 *     - plmn
 *     - nci
 *     - pci
 *     - tac
 *     - crnti
 *
 * CU-CP (Central Unit - Control Plane):
 * -------------------------------------
 * The function `cucp_uemgr_ue_add` is called when a UE is created at the CU-CP.
 * Parameters:
 *     - cu_cp_ue_index
 *     - plmn
 *     - pci
 *     - rnti
 *
 * The `cu_cp_ue_index` can be mapped to a `du_ue_index` based on matching (plmn, pci, rnti).
 *
 * CU-UP (Central Unit - User Plane):
 * ----------------------------------
 * The UE context in the CU-UP is set up via the E1AP Bearer Setup procedure.
 * The following hooks are invoked:
 *
 *     - `e1_cucp_bearer_context_setup`
 *         Parameters: cu_cp_ue_index, gnb_cu_cp_ue_e1ap_id
 *
 *     - `e1_cuup_bearer_context_setup`
 *         Parameters: cu_cp_ue_index, gnb_cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id
 *
 * Mapping details:
 *     - `gnb_cu_cp_ue_e1ap_id` corresponds to `cu_cp_ue_index`
 *     - (`gnb_cu_cp_ue_e1ap_id`, `gnb_cu_up_ue_e1ap_id`) together identify the `cu_up_ue_index`
 *
 * PDCP (Packet Data Convergence Protocol):
 * ----------------------------------------
 * PDCP hooks are invoked in both CU-CP (for SRBs) and CU-UP (for DRBs), and share a unified 
 * context structure:
 *
 *     struct jbpf_pdcp_ctx_info {
 *         uint16_t ctx_id;       // Context ID (implementation specific)
 *         uint32_t cu_ue_index;  // SRB: cu_cp_ue_index, DRB: cu_up_ue_index
 *         uint8_t  is_srb;       // true = SRB, false = DRB
 *         ....
 *     };
 *
 * Notes:
 *     - In CU-CP, hooks are called for SRBs. Thus, `cu_ue_index` refers to `cu_cp_ue_index`.
 *     - In CU-UP, hooks are called for DRBs. Thus, `cu_ue_index` refers to `cu_up_ue_index`.
 */



#ifdef __cplusplus
extern "C"
{
#endif

#include "jbpf_hook.h"
#include "jbpf_srsran_defs.h"
#include "jbpf_srsran_contexts.h"

DECLARE_JBPF_HOOK(capture_xran_packet,
		   struct jbpf_ran_ofh_ctx ctx,
		   ctx,
		   HOOK_PROTO(
			      const void* xran_packet,
			      uint32_t packet_len, 
			      uint16_t ctx_id,
				  JbpfDirection_t direction
			      ),
		   HOOK_ASSIGN(
			       ctx.ctx_id = ctx_id;
			       ctx.data = (uint64_t)xran_packet;
			       ctx.data_end = (uint64_t) ((uint8_t *)xran_packet + packet_len);
				   ctx.direction = direction;
			       )
		   )



#define DECLARE_JBPF_FAPI_HOOK(name) \
    DECLARE_JBPF_HOOK(name, \
        struct jbpf_ran_layer2_ctx ctx, \
        ctx, \
        HOOK_PROTO( \
            void *ind, \
            uint16_t ctx_id, \
            uint16_t cell_id, \
            size_t payload_size \
        ), \
        HOOK_ASSIGN( \
            ctx.ctx_id = ctx_id; \
            ctx.cell_id = cell_id; \
            ctx.data = (uint64_t)ind; \
            ctx.data_end = (uint64_t) ((uint8_t*)ind + payload_size); \
        ) \
    )

// FAPI phy-to-mac hooks
DECLARE_JBPF_FAPI_HOOK(fapi_rx_data_indication);
DECLARE_JBPF_FAPI_HOOK(fapi_crc_indication);
DECLARE_JBPF_FAPI_HOOK(fapi_uci_indication);
DECLARE_JBPF_FAPI_HOOK(fapi_srs_indication);
DECLARE_JBPF_FAPI_HOOK(fapi_rach_indication);

// FAPI mac-to-phy hooks
DECLARE_JBPF_FAPI_HOOK(fapi_dl_tti_request);
DECLARE_JBPF_FAPI_HOOK(fapi_ul_tti_request);
DECLARE_JBPF_FAPI_HOOK(fapi_ul_dci_request);
DECLARE_JBPF_FAPI_HOOK(fapi_tx_data_request);



// DU UE creation/deletion

DECLARE_JBPF_HOOK(du_ue_ctx_creation,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_du_ue_ctx_info *ue_info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)ue_info;
        ctx.data_end = (uint64_t) ((uint8_t*)ue_info + sizeof(struct jbpf_du_ue_ctx_info));
    )
)

DECLARE_JBPF_HOOK(du_ue_ctx_update_crnti,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_du_ue_ctx_info *ue_info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)ue_info;
        ctx.data_end = (uint64_t) ((uint8_t*)ue_info + sizeof(struct jbpf_du_ue_ctx_info));
    )
)

DECLARE_JBPF_HOOK(du_ue_ctx_deletion,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_du_ue_ctx_info *ue_info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)ue_info;
        ctx.data_end = (uint64_t) ((uint8_t*)ue_info + sizeof(struct jbpf_du_ue_ctx_info));
    )
)


// MAC scheduler

#define DECLARE_MAC_SCHED_HOOK(name) \
    DECLARE_JBPF_HOOK(name, \
        struct jbpf_mac_sched_ctx ctx, \
        ctx, \
        HOOK_PROTO( \
            void *ind, \
            uint16_t ctx_id, \
            uint16_t du_ue_index, \
            uint16_t cell_id, \
            uint16_t rnti, \
            size_t payload_size \
        ), \
        HOOK_ASSIGN( \
            ctx.ctx_id = ctx_id; \
            ctx.du_ue_index = du_ue_index; \
            ctx.data = (uint64_t)ind; \
            ctx.data_end = (uint64_t) ((uint8_t*)ind + payload_size); \
        ) \
    )

#define DECLARE_MAC_SCHED_HOOK_NO_PAYLOAD(name) \
    DECLARE_JBPF_HOOK(name, \
        struct jbpf_mac_sched_ctx ctx, \
        ctx, \
        HOOK_PROTO( \
            uint16_t ctx_id, \
            uint16_t du_ue_index, \
            uint16_t cell_id, \
            uint16_t rnti \
        ), \
        HOOK_ASSIGN( \
            ctx.ctx_id = ctx_id; \
            ctx.du_ue_index = du_ue_index; \
            ctx.data = (uint64_t)0; \
            ctx.data_end = (uint64_t)0; \
        ) \
    )

DECLARE_MAC_SCHED_HOOK_NO_PAYLOAD(mac_sched_ue_creation);
DECLARE_MAC_SCHED_HOOK_NO_PAYLOAD(mac_sched_ue_reconfig);
DECLARE_MAC_SCHED_HOOK_NO_PAYLOAD(mac_sched_ue_deletion);
DECLARE_MAC_SCHED_HOOK_NO_PAYLOAD(mac_sched_ue_config_applied);
DECLARE_MAC_SCHED_HOOK(mac_sched_ul_bsr_indication);           /* ctx.data will be a srsran::ul_bsr_indication_message */
DECLARE_MAC_SCHED_HOOK(mac_sched_uci_indication);              /* ctx.data will be a srsran::uci_indication::uci_pdu */
DECLARE_MAC_SCHED_HOOK(mac_sched_dl_mac_ce_indication);        /* ctx.data will be a srsran::dl_mac_ce_indication */
DECLARE_MAC_SCHED_HOOK(mac_sched_ul_phr_indication);           /* ctx.data will be a srsran::ul_phr_indication_message */
DECLARE_MAC_SCHED_HOOK(mac_sched_dl_buffer_state_indication);  /* ctx.data will be a srsran::dl_buffer_state_indication_message */
DECLARE_MAC_SCHED_HOOK(mac_sched_srs_indication);              /* ctx.data will be a srsran::srs_indication::srs_indication_pdu */

DECLARE_JBPF_HOOK(mac_sched_crc_indication,
    struct jbpf_mac_sched_ctx ctx, 
    ctx, 
    HOOK_PROTO( 
        void* ind,                         /* will be a srsran::ul_crc_pdu_indication */
        uint16_t ctx_id, 
        uint16_t du_ue_index, 
        uint16_t cell_id, 
        uint16_t rnti,
        bool processed, 
        uint nof_retxs,
        uint max_nof_retxs,
        size_t payload_size),
    HOOK_ASSIGN( \
        ctx.ctx_id = ctx_id; 
        ctx.du_ue_index = du_ue_index; 
        ctx.data = (uint64_t)ind;
        ctx.data_end = (uint64_t) ((uint8_t*)ind + payload_size); 
        ctx.srs_meta_data1 = (uint64_t)processed << 32 | (uint64_t)nof_retxs << 16 | (uint64_t)max_nof_retxs;
    ) 
)

#define DECLARE_MAC_SCHED_HARQ_HOOK(name, harq_ctx_type) \
    DECLARE_JBPF_HOOK(name, \
        struct jbpf_mac_sched_ctx ctx, \
        ctx, \
        HOOK_PROTO( \
            struct harq_ctx_type *harq_info, \
            uint16_t ctx_id, \
            uint16_t cell_id, \
            uint16_t du_ue_index, \
            uint16_t rnti), \
        HOOK_ASSIGN( \
            ctx.ctx_id = ctx_id; \
            ctx.cell_id = cell_id; \
            ctx.du_ue_index = du_ue_index; \
            ctx.rnti = rnti; \
            ctx.data = (uint64_t)harq_info; \
            ctx.data_end = (uint64_t) ((uint8_t*)harq_info + sizeof(*harq_info)); \
        ) \
    )

DECLARE_MAC_SCHED_HARQ_HOOK(mac_sched_harq_ul, jbpf_mac_sched_harq_ctx_info);
DECLARE_MAC_SCHED_HARQ_HOOK(mac_sched_harq_dl, jbpf_mac_sched_harq_ctx_info_dl);

// PDCP

// trigger: when PDCP DL entity is created
DECLARE_JBPF_HOOK(pdcp_dl_creation,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
    )
)

// trigger: when PDCP DL entity is deleted
DECLARE_JBPF_HOOK(pdcp_dl_deletion,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
    )
)

// trigger: when new SDU is received from higher layers
DECLARE_JBPF_HOOK(pdcp_dl_new_sdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer,
        uint32_t count,
        uint32_t sdu_length),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)count << 32 | sdu_length; 
    )
)

// trigger: when a PDCP PDU is sent to lower layers
DECLARE_JBPF_HOOK(pdcp_dl_tx_data_pdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer,
        uint32_t pdu_length,
        uint32_t count,
        uint8_t is_retx,
        uint8_t latency_set,
        uint64_t latency_ns), // from sdu-arrival to transmission of the PDU
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)pdu_length << 32 | count;
        ctx.srs_meta_data2 = (uint64_t)is_retx << 32 | latency_set;
        ctx.srs_meta_data3 = latency_ns; 
    )
)

// trigger: when a control PDU is sent to lower layers
DECLARE_JBPF_HOOK(pdcp_dl_tx_control_pdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer,
        uint32_t pdu_length),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)pdu_length;
    )
)

// trigger: when the first byte of an SDU is sent to lower layers
// Note that the notif_count means "up to and including" that count. i.e. in the following example
//             pdcp_dl_handle_tx_notification notif_count=0
//             pdcp_dl_handle_tx_notification notif_count=1
//             pdcp_dl_handle_tx_notification notif_count=5
// the last message means that counts 2-5 are all being notified.
DECLARE_JBPF_HOOK(pdcp_dl_handle_tx_notification,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer,
        uint32_t notif_sn),         
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
        ctx.srs_meta_data1 = notif_sn;
    )
)

// RLC TM/UM mode, trigger: when the SDU is completely sent to lower layers.
// RLM AM mode, trigger: when all of the PDU used to transmit an SDU have 
// been acknowledged by the UE.
// Note that the notif_count means "up to and including" that count. i.e. in the following example
//             pdcp_dl_handle_delivery_notification notif_count=0
//             pdcp_dl_handle_delivery_notification notif_count=1
//             pdcp_dl_handle_delivery_notification notif_count=5
// the last message means that counts 2-5 are all being notified.
DECLARE_JBPF_HOOK(pdcp_dl_handle_delivery_notification,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer,
        uint32_t notif_sn), 
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
        ctx.srs_meta_data1 = notif_sn; 
    )
)

// trigger: when an SDU is discarded by the PDCP layer
DECLARE_JBPF_HOOK(pdcp_dl_discard_pdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer,
        uint32_t count),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
        ctx.srs_meta_data1 = count;
    )
)

// trigger: when a PDCP bearer is re-established
DECLARE_JBPF_HOOK(pdcp_dl_reestablish,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
    )
)

// trigger: when a PDU is received from lower layers
DECLARE_JBPF_HOOK(pdcp_ul_rx_data_pdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer,
        uint32_t sdu_length,
        uint32_t header_length,
        uint32_t count),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)sdu_length << 32 | header_length;
        ctx.srs_meta_data2 = (uint64_t)count;
    )
)

// trigger: when a control PDU is received from lower layers
DECLARE_JBPF_HOOK(pdcp_ul_rx_control_pdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer,
        uint32_t pdu_length),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)pdu_length;
    )
)

// trigger: when PDCP UL entity is created
DECLARE_JBPF_HOOK(pdcp_ul_creation,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
    )
)

// trigger: when PDCP UL entity is deleted
DECLARE_JBPF_HOOK(pdcp_ul_deletion,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
    )
)

// trigger: when an SDU is delivered to higher layers
DECLARE_JBPF_HOOK(pdcp_ul_deliver_sdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer,
        uint32_t sdu_length),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)sdu_length ;
    )
)

// trigger: when a PDCP bearer is re-established
DECLARE_JBPF_HOOK(pdcp_ul_reestablish,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_pdcp_ctx_info *bearer),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)bearer;
        ctx.data_end = (uint64_t) ((uint8_t*)bearer + sizeof(struct jbpf_pdcp_ctx_info));
    )
)


 // E1AP

DECLARE_JBPF_HOOK(e1_cucp_bearer_context_setup,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_cucp_e1_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_cucp_e1_ctx_info));
    )
)

DECLARE_JBPF_HOOK(e1_cucp_bearer_context_modification,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_cucp_e1_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_cucp_e1_ctx_info));
    )
)

DECLARE_JBPF_HOOK(e1_cucp_bearer_context_release,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_cucp_e1_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_cucp_e1_ctx_info));
    )
)

DECLARE_JBPF_HOOK(e1_cucp_bearer_context_inactivity,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_cucp_e1_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_cucp_e1_ctx_info));
    )
)

DECLARE_JBPF_HOOK(e1_cuup_bearer_context_setup,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_cuup_e1_ctx_info *info,
        uint8_t success),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_cuup_e1_ctx_info));
        ctx.srs_meta_data1 = success;
    )
)

DECLARE_JBPF_HOOK(e1_cuup_bearer_context_modification,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_cuup_e1_ctx_info *info,
        uint8_t success),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_cuup_e1_ctx_info));
        ctx.srs_meta_data1 = success;
    )
)

DECLARE_JBPF_HOOK(e1_cuup_bearer_context_release,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_cuup_e1_ctx_info *info,
        uint8_t success),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_cuup_e1_ctx_info));
        ctx.srs_meta_data1 = success;
    )
)


/* CUCP UE Manager */

DECLARE_JBPF_HOOK(cucp_uemgr_ue_add,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_cucp_uemgr_ctx_info *info,
        bool pci_set,
        uint16_t pci,
        bool rnti_set,
        uint16_t rnti),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_cucp_uemgr_ctx_info));
        ctx.srs_meta_data1 = ((uint64_t)pci_set) << 16 | pci;
        ctx.srs_meta_data2 = (uint64_t)rnti_set << 16 | rnti;
    )
)

DECLARE_JBPF_HOOK(cucp_uemgr_ue_update,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_cucp_uemgr_ctx_info *info,
        uint16_t pci,
        uint16_t rnti),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_cucp_uemgr_ctx_info));
        ctx.srs_meta_data1 = pci;
        ctx.srs_meta_data2 = rnti;
    )
)

DECLARE_JBPF_HOOK(cucp_uemgr_ue_remove,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_cucp_uemgr_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_cucp_uemgr_ctx_info));
    )
)

/* NGAP */

DECLARE_JBPF_HOOK(ngap_procedure_started,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_ngap_ctx_info *info,
        JbpfNgapProcedure_t procedure,
        uint64_t meta),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_ngap_ctx_info));
        ctx.srs_meta_data1 = procedure;
        ctx.srs_meta_data2 = meta;
    )
)

DECLARE_JBPF_HOOK(ngap_procedure_completed,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_ngap_ctx_info *info,
        JbpfNgapProcedure_t procedure,
        bool success,
        uint64_t meta),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_ngap_ctx_info));
        ctx.srs_meta_data1 = ((uint64_t)success << 32) | procedure;
        ctx.srs_meta_data2 = meta;
    )
)
 
DECLARE_JBPF_HOOK(ngap_reset,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_ngap_ctx_info *info,
        uint64_t meta),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_ngap_ctx_info));
        ctx.srs_meta_data2 = meta;
    )
)



/* RRC */

DECLARE_JBPF_HOOK(rrc_ue_add,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rrc_ctx_info *info,
        uint16_t c_rnti,
        uint16_t pci,
        uint32_t tac,
        uint32_t plmn,
        uint64_t nci),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rrc_ctx_info));
        ctx.srs_meta_data1 = ((uint64_t)c_rnti << 48) | ((uint64_t)pci << 32) | tac;
        ctx.srs_meta_data2 = plmn;
        ctx.srs_meta_data3 = nci;
    )
)

DECLARE_JBPF_HOOK(rrc_ue_update_context,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rrc_ctx_info *info,
        uint64_t old_cu_ue_index,
        uint16_t c_rnti,
        uint16_t pci,
        uint32_t tac,
        uint32_t plmn,
        uint64_t nci),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rrc_ctx_info));
        ctx.srs_meta_data1 = old_cu_ue_index;
        ctx.srs_meta_data2 = ((uint64_t)c_rnti << 48) | ((uint64_t)pci << 32) | tac;
        ctx.srs_meta_data3 = plmn;
        ctx.srs_meta_data4 = nci;
    )
)

DECLARE_JBPF_HOOK(rrc_ue_update_id,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rrc_ctx_info *info,
        uint64_t _5gtimsi),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rrc_ctx_info));
        ctx.srs_meta_data1 =_5gtimsi;
    )
)

DECLARE_JBPF_HOOK(rrc_ue_remove,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rrc_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rrc_ctx_info));
   )
)

DECLARE_JBPF_HOOK(rrc_ue_procedure_started,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rrc_ctx_info *info,
        JbpfRrcProcedure_t procedure,
        uint64_t meta),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rrc_ctx_info));
        ctx.srs_meta_data1 = procedure;
        ctx.srs_meta_data2 = meta;
    )
)

DECLARE_JBPF_HOOK(rrc_ue_procedure_completed,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rrc_ctx_info *info,
        JbpfRrcProcedure_t procedure,
        bool success,
        uint64_t meta),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rrc_ctx_info));
        ctx.srs_meta_data1 = ((uint64_t)success << 32) | procedure;
        ctx.srs_meta_data2 = meta;
    )
)

// trigger: when RLC DL entity is created
DECLARE_JBPF_HOOK(rlc_dl_creation,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
    )
)

// trigger: when RLC DL entity is deleted
DECLARE_JBPF_HOOK(rlc_dl_deletion,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
    )
)

// trigger: SDU received from upper layer
DECLARE_JBPF_HOOK(rlc_dl_new_sdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        uint32_t sdu_length,
        uint32_t pdcp_sn,
        uint8_t is_retx),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)sdu_length << 32 | pdcp_sn;
        ctx.srs_meta_data2 = is_retx; // is_retx indicates if this SDU is a retransmission
    )
)

// trigger: SDU received from upper layer which is dropped
DECLARE_JBPF_HOOK(rlc_dl_lost_sdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        uint32_t sdu_length,
        uint32_t pdcp_sn,
        uint8_t is_retx),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)sdu_length << 32 | pdcp_sn;
        ctx.srs_meta_data2 = is_retx; // is_retx indicates if this SDU is a retransmission
    )
)

// trigger: SDU discarded
DECLARE_JBPF_HOOK(rlc_dl_discard_sdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        uint32_t pdcp_sn,
        bool success),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)pdcp_sn << 32 | success;
    )
)

// trigger: transmission of the SDU starts
DECLARE_JBPF_HOOK(rlc_dl_sdu_send_started,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        uint32_t pdcp_sn,
        bool is_retx,
        uint64_t latency_ns),   /* this is ns from sdu-arrival time to start of SDU transmission*/
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)pdcp_sn << 32 | is_retx;
        ctx.srs_meta_data2 = latency_ns; 
    )
)

// trigger: all bytes of the SDU have been transmitted
DECLARE_JBPF_HOOK(rlc_dl_sdu_send_completed,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        uint32_t pdcp_sn,
        bool is_retx,
        uint64_t latency_ns),   /* this is ns from sdu-arrival time to end of SDU transmission*/
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)pdcp_sn << 32 | is_retx;
        ctx.srs_meta_data2 = latency_ns; 
    )
)

// trigger: all bytes of the SDU have been received by the peer RLC entity
DECLARE_JBPF_HOOK(rlc_dl_sdu_delivered,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        uint32_t pdcp_sn,
        bool is_retx,
        uint64_t latency_ns),   /* this is ns from sdu-arrival time to SDU delivery notification*/
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)pdcp_sn << 32 | is_retx;
        ctx.srs_meta_data2 = latency_ns; 
    )
)

// trigger: when a PDU is delivered to lower layers for transmission
DECLARE_JBPF_HOOK(rlc_dl_tx_pdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        JbpfRlcPdu_t pdu_type,
        uint32_t pdu_len),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)pdu_type << 32 | pdu_len;
    )
)

// trigger: when a STATUS PDU is received from lower layers
DECLARE_JBPF_HOOK(rlc_dl_rx_status,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
    )
)


// trigger: when the retx count of an AM PDU is updated.
DECLARE_JBPF_HOOK(rlc_dl_am_tx_pdu_retx_count,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        uint32_t sn,
        uint32_t retx_count),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)sn << 32 | retx_count;
    )
)

// trigger: when the retx count of an AM PDU reaches the maximum
DECLARE_JBPF_HOOK(rlc_dl_am_tx_pdu_max_retx_count_reached,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        uint32_t sn,
        uint32_t retx_count),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)sn << 32 | retx_count;
    )
)

// trigger: when RLC UL entity is created
DECLARE_JBPF_HOOK(rlc_ul_creation,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
    )
)

// trigger: when RLC UL entity is deleted
DECLARE_JBPF_HOOK(rlc_ul_deletion,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
    )
)

// trigger: when a PDU is received from lower layers
DECLARE_JBPF_HOOK(rlc_ul_rx_pdu,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        JbpfRlcPdu_t pdu_type,
        uint32_t pdu_len),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)pdu_type << 32 | pdu_len;
    )
)

// trigger: when a PDU is received for an SDU for which no bytes have previously been received.
DECLARE_JBPF_HOOK(rlc_ul_sdu_recv_started,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        uint32_t sn),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)sn;
    )
)

// trigger: when an SDU is delivered to higher layers
DECLARE_JBPF_HOOK(rlc_ul_sdu_delivered,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctx_info *info,
        uint32_t sn,
        uint32_t sdu_length,
        uint64_t latency_ns), // from start of SDU reception to SDU delivery
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctx_info));
        ctx.srs_meta_data1 = (uint64_t)sn << 32 | sdu_length;
        ctx.srs_meta_data2 = latency_ns;
    )
)

/* CONTROL hook: dynamic RLC DL buffer management.
 * Fires once per DL SDU handed to an RLC AM DRB TX entity (not SRBs), before the
 * enqueue/tail-drop decision. ctx.data points at an srsRAN-owned jbpf_rlc_ctrl_info;
 * the codelet may WRITE info->new_byte_limit / new_sdu_limit, which srsRAN clamps
 * and applies to the SDU queue.
 * Paired with DEFINE_JBPF_CTRL_HOOK(rlc_dl_ctrl) in rlc_tx_am_entity.cpp. */
DECLARE_JBPF_HOOK(rlc_dl_ctrl,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_rlc_ctrl_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_rlc_ctrl_info));
    )
)

/* CONTROL hook: EdgeRIC-style DL scheduling (Max-Weight). Fires per-UE per-slot
 * at the end of scheduler_time_pf::ue_ctxt::compute_dl_prio. ctx.data points at
 * an srsRAN-owned jbpf_mac_sched_dl_ctrl_info carrying per-UE telemetry (rnti,
 * tti, cqi, dl_backlog_bytes, pf_prio); a control codelet may WRITE new_prio_bits
 * + new_prio_valid to override the PF priority used to order the DL queue.
 * Paired with DEFINE_JBPF_CTRL_HOOK(mac_sched_dl_ctrl) in scheduler_time_pf.cpp. */
DECLARE_JBPF_HOOK(mac_sched_dl_ctrl,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_mac_sched_dl_ctrl_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_mac_sched_dl_ctrl_info));
    )
)

/* CONTROL hook: per-UE DL MCS override. Fires per newTx UE at the PDSCH
 * MCS-selection (link-adaptation) point in grant_params_selector. ctx.data points
 * at an srsRAN-owned jbpf_mac_sched_dl_mcs_ctrl_info carrying per-UE telemetry
 * (rnti, tti, cqi, mcs_table, cur_mcs); a control codelet may set new_mcs_valid +
 * new_mcs to override the link-adaptation MCS used for this UE's PDSCH grant.
 * Paired with DEFINE_JBPF_CTRL_HOOK(mac_sched_dl_mcs_ctrl) in grant_params_selector.cpp. */
DECLARE_JBPF_HOOK(mac_sched_dl_mcs_ctrl,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        struct jbpf_mac_sched_dl_mcs_ctrl_info *info),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)info;
        ctx.data_end = (uint64_t) ((uint8_t*)info + sizeof(struct jbpf_mac_sched_dl_mcs_ctrl_info));
    )
)

/* L4Span: DL SDU marking hook. ctx.data points at the WRITABLE first segment of
 * the DL IP packet (the IP header + start of payload). A control codelet may
 * rewrite the ECN bits (IP TOS byte) + IP checksum in place; the modified packet
 * is what gets sent to lower layers. meta1 = ue_index<<32 | rb_id, meta2 = count.
 * Paired with DEFINE_JBPF_CTRL_HOOK(pdcp_dl_sdu_segment) in pdcp_entity_tx.cpp. */
DECLARE_JBPF_HOOK(pdcp_dl_sdu_segment,
    struct jbpf_ran_generic_ctx ctx,
    ctx,
    HOOK_PROTO(
        uint8_t* pkt,
        uint32_t pkt_len,
        uint32_t rb_id,
        uint32_t ue_index,
        uint32_t count),
    HOOK_ASSIGN(
        ctx.data = (uint64_t)pkt;
        ctx.data_end = (uint64_t)(pkt + pkt_len);
        ctx.srs_meta_data1 = ((uint64_t)ue_index << 32) | rb_id;
        ctx.srs_meta_data2 = count;
    )
)


#ifdef __cplusplus
}
#endif


#endif // JBPF_SRSRAN_HOOKS_
