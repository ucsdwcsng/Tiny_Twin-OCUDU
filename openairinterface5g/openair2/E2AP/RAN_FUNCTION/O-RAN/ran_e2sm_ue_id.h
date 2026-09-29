/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef RAN_E2SM_UE_ID_H
#define RAN_E2SM_UE_ID_H

#include "openair2/E2AP/flexric/src/agent/../sm/sm_io.h"
#include "openair2/RRC/NR/rrc_gNB_UE_context.h"

ue_id_e2sm_t fill_e2sm_gnb_ue_id_data(const gNB_RRC_UE_t *rrc_ue_context, const uint32_t rrc_ue_id, const ue_id_t cucp_ue_id);

ue_id_e2sm_t fill_e2sm_cu_ue_id_data(const gNB_RRC_UE_t *rrc_ue_context, const uint32_t rrc_ue_id, const ue_id_t cucp_ue_id);

ue_id_e2sm_t fill_e2sm_du_ue_id_data(const gNB_RRC_UE_t *rrc_ue_context, const uint32_t rrc_ue_id, const ue_id_t cucp_ue_id);

ue_id_e2sm_t fill_e2sm_cucp_ue_id_data(const gNB_RRC_UE_t *rrc_ue_context, const uint32_t rrc_ue_id, const ue_id_t cucp_ue_id);

ue_id_e2sm_t fill_e2sm_cuup_ue_id_data(const gNB_RRC_UE_t *rrc_ue_context, const uint32_t rrc_ue_id, const ue_id_t cucp_ue_id);

typedef ue_id_e2sm_t (*get_ue_id)(const gNB_RRC_UE_t *rrc_ue_context, const uint32_t rrc_ue_id, const ue_id_t cucp_ue_id);

extern get_ue_id fill_ue_id_data[END_NGRAN_NODE_TYPE];

#endif
