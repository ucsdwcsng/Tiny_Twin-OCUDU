/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief function prototypes for FAPI L1 interface
 */

#include "PHY/defs_eNB.h"
#include "PHY/LTE_TRANSPORT/transport_proto.h"
#include "SCHED/sched_eNB.h"
#include "SCHED/sched_common.h"
#include "nfapi_interface.h"

void fill_uci_harq_indication(int UEid, PHY_VARS_eNB *eNB,LTE_eNB_UCI *uci,int frame,int subframe,uint8_t *harq_ack,uint8_t tdd_mapping_mode,uint16_t tdd_multiplexing_mask);
void fill_ulsch_harq_indication(PHY_VARS_eNB *eNB,LTE_UL_eNB_HARQ_t *ulsch_harq,uint16_t rnti, int frame,int subframe,int bundling);
void fill_ulsch_cqi_indication(PHY_VARS_eNB *eNB,uint16_t frame,uint8_t subframe,LTE_UL_eNB_HARQ_t *ulsch_harq,uint16_t rnti);
void fill_sr_indication(int UEid, PHY_VARS_eNB *eNB,uint16_t rnti,int frame,int subframe,uint32_t stat);
void fill_rx_indication(PHY_VARS_eNB *eNB,int UE_id,int frame,int subframe);
void fill_crc_indication(PHY_VARS_eNB *eNB,int UE_id,int frame,int subframe,uint8_t crc_flag);
void handle_nfapi_dci_dl_pdu(PHY_VARS_eNB *eNB,int frame,int subframe,L1_rxtx_proc_t *proc,nfapi_dl_config_request_pdu_t *dl_config_pdu);
void handle_nfapi_mpdcch_pdu(PHY_VARS_eNB *eNB,L1_rxtx_proc_t *proc,nfapi_dl_config_request_pdu_t *dl_config_pdu);
void handle_nfapi_hi_dci0_dci_pdu(PHY_VARS_eNB *eNB,int frame,int subframe,L1_rxtx_proc_t *proc,
				  nfapi_hi_dci0_request_pdu_t *hi_dci0_config_pdu);
void handle_nfapi_hi_dci0_hi_pdu(PHY_VARS_eNB *eNB,int frame,int subframe,L1_rxtx_proc_t *proc,
				 nfapi_hi_dci0_request_pdu_t *hi_dci0_config_pdu);
void handle_nfapi_dlsch_pdu(PHY_VARS_eNB *eNB,int frame,int subframe,L1_rxtx_proc_t *proc,
			    nfapi_dl_config_request_pdu_t *dl_config_pdu,
			    uint8_t codeword_index,
			    uint8_t *sdu);

void handle_nfapi_mch_pdu(PHY_VARS_eNB *eNB,int frame,int subframe,L1_rxtx_proc_t *proc,
			    nfapi_dl_config_request_pdu_t *dl_config_pdu,
			    uint8_t *sdu);

void handle_nfapi_ul_pdu(PHY_VARS_eNB *eNB,L1_rxtx_proc_t *proc,
			 nfapi_ul_config_request_pdu_t *ul_config_pdu,
			 uint16_t frame,uint8_t subframe,uint8_t srs_present);

void handle_nfapi_hi_dci0_dci_pdu(PHY_VARS_eNB *eNB,int frame,int subframe,L1_rxtx_proc_t *proc,
                                  nfapi_hi_dci0_request_pdu_t *hi_dci0_config_pdu);
void handle_ulsch_harq_pdu(
        PHY_VARS_eNB                           *eNB,
        int                                     UE_id,
        nfapi_ul_config_request_pdu_t          *ul_config_pdu,
        nfapi_ul_config_ulsch_harq_information *harq_information,
        uint16_t                                frame,
        uint8_t                                 subframe);
void handle_nfapi_bch_pdu(PHY_VARS_eNB *eNB,L1_rxtx_proc_t *proc,
                          nfapi_dl_config_request_pdu_t *dl_config_pdu,
                          uint8_t *sdu) ;
void handle_ulsch_cqi_ri_pdu(PHY_VARS_eNB *eNB,int UE_id,nfapi_ul_config_request_pdu_t *ul_config_pdu,uint16_t frame,uint8_t subframe);

void handle_uci_harq_information(PHY_VARS_eNB *eNB, LTE_eNB_UCI *uci,nfapi_ul_config_harq_information *harq_information);

void handle_uci_sr_pdu(PHY_VARS_eNB *eNB,int UE_id,nfapi_ul_config_request_pdu_t *ul_config_pdu,uint16_t frame,uint8_t subframe,uint8_t srs_active);

void handle_uci_sr_harq_pdu(PHY_VARS_eNB *eNB,int UE_id,nfapi_ul_config_request_pdu_t *ul_config_pdu,uint16_t frame,uint8_t subframe,uint8_t srs_active);
void handle_uci_harq_pdu(PHY_VARS_eNB *eNB,int UE_id,nfapi_ul_config_request_pdu_t *ul_config_pdu,uint16_t frame,uint8_t subframe,uint8_t srs_active);

void handle_srs_pdu(PHY_VARS_eNB *eNB,nfapi_ul_config_request_pdu_t *ul_config_pdu,uint16_t frame,uint8_t subframe);

void schedule_response(Sched_Rsp_t *Sched_INFO, void *proc);
