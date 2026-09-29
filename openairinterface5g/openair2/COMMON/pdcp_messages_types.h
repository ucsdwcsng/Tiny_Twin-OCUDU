/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*
 * pdcp_messages_types.h
 */

#ifndef PDCP_MESSAGES_TYPES_H_
#define PDCP_MESSAGES_TYPES_H_

//-------------------------------------------------------------------------------------------//
// Defines to access message fields.
#define RRC_DCCH_DATA_REQ(mSGpTR)               (mSGpTR)->ittiMsg.rrc_dcch_data_req
#define RRC_DCCH_DATA_IND(mSGpTR)               (mSGpTR)->ittiMsg.rrc_dcch_data_ind
#define RRC_PCCH_DATA_REQ(mSGpTR)               (mSGpTR)->ittiMsg.rrc_pcch_data_req
#define RRC_DCCH_DATA_COPY_IND(mSGpTR)          (mSGpTR)->ittiMsg.rrc_dcch_data_copy_ind

// gNB
#define NR_RRC_DCCH_DATA_REQ(mSGpTR)            (mSGpTR)->ittiMsg.nr_rrc_dcch_data_req
#define NR_RRC_DCCH_DATA_IND(mSGpTR)            (mSGpTR)->ittiMsg.nr_rrc_dcch_data_ind

//-------------------------------------------------------------------------------------------//
// Messages between RRC and PDCP layers
typedef struct RrcDcchDataReq_s {
  uint32_t frame;
  uint8_t enb_flag;
  rb_id_t rb_id;
  uint32_t muip;
  uint32_t confirmp;
  uint32_t sdu_size;
  uint8_t *sdu_p;
  uint8_t mode;
  uint16_t     rnti;
  uint8_t      module_id;
  uint8_t eNB_index;
} RrcDcchDataReq;

typedef struct RrcDcchDataInd_s {
  uint32_t frame;
  uint8_t dcch_index;
  uint32_t sdu_size;
  uint8_t *sdu_p;
  uint16_t     rnti;
  uint8_t      module_id;
  uint8_t      eNB_index; // LG: needed in UE
} RrcDcchDataInd;

typedef struct RrcDcchDataCopyInd_s {
  uint8_t dcch_index;
  uint32_t sdu_size;
  uint8_t *sdu_p;
  uint8_t      eNB_index;
} RrcDcchDataCopyInd;

typedef struct NRRrcDcchDataReq_s {
  uint32_t frame;
  uint8_t  gnb_flag;
  rb_id_t  rb_id;
  uint32_t muip;
  uint32_t confirmp;
  uint32_t sdu_size;
  uint8_t *sdu_p;
  uint8_t  mode;
  uint16_t rnti;
  uint8_t  module_id;
  uint8_t  gNB_index;
} NRRrcDcchDataReq;

#include "openair2/LAYER2/nr_pdcp/nr_pdcp_integrity_data.h"

typedef struct NRRrcDcchDataInd_s {
  uint32_t frame;
  uint8_t dcch_index;
  uint32_t sdu_size;
  uint8_t *sdu_p;
  uint16_t rnti;
  uint8_t module_id;
  uint8_t gNB_index; // LG: needed in UE
  /* 'msg_integrity' is needed for RRC to check integrity of the PDCP SDU */
  nr_pdcp_integrity_data_t msg_integrity;
} NRRrcDcchDataInd;

typedef struct RrcPcchDataReq_s {
  uint32_t     sdu_size;
  uint8_t      *sdu_p;
  uint8_t      mode;
  uint16_t     rnti;
  uint8_t      ue_index;
  uint8_t      CC_id;
} RrcPcchDataReq;

#endif /* PDCP_MESSAGES_TYPES_H_ */
