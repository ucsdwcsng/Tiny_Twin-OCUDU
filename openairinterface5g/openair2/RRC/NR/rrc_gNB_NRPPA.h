/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef RRC_GNB_NRPPA_H_
#define RRC_GNB_NRPPA_H_

#include "nr_rrc_defs.h"
#include "openair2/COMMON/nrppa_messages_types.h"
#include "openair2/COMMON/f1ap_messages_types.h"

void rrc_gNB_process_trp_information_request(gNB_RRC_INST *rrc, const nrppa_trp_information_req_t *msg);
void rrc_CU_process_trp_information_response(f1ap_trp_information_resp_t *f1ap_msg);
void rrc_gNB_process_positioning_information_request(gNB_RRC_INST *rrc, const nrppa_positioning_information_req_t *msg);
void rrc_CU_process_positioning_information_response(f1ap_positioning_information_resp_t *f1ap_msg);
void rrc_gNB_process_positioning_activation_request(gNB_RRC_INST *rrc, const nrppa_positioning_activation_req_t *msg);
void rrc_CU_process_positioning_activation_response(f1ap_positioning_activation_resp_t *f1ap_msg);
void rrc_gNB_process_positioning_measurement_request(gNB_RRC_INST *rrc, const nrppa_measurement_req_t *msg);
void rrc_CU_process_positioning_measurement_response(f1ap_positioning_measurement_resp_t *f1ap_msg);
void rrc_CU_process_trp_information_failure(f1ap_trp_information_failure_t *f1ap_msg);
void rrc_CU_process_positioning_measurement_failure(f1ap_positioning_measurement_failure_t *f1ap_msg);

#endif /* RRC_GNB_NRPPA_H_ */
