/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NRPPA_GNB_MEASUREMENT_INFORMATION_TRANSFER_H_
#define NRPPA_GNB_MEASUREMENT_INFORMATION_TRANSFER_H_

#include "nrppa_common.h"

int nrppa_gNB_handle_measurement_request(nrppa_gnb_ue_info_t *nrppa_msg_info, const NRPPA_NRPPA_PDU_t *pdu);
void decode_srs_carrier_list(nrppa_srs_carrier_list_t *out_list, const NRPPA_SRSCarrier_List_t *in_list);
void free_nrppa_measurement_request(nrppa_measurement_req_t *msg);
int nrppa_gNB_measurement_response(instance_t instance, MessageDef *msg_p);
NRPPA_TRP_MeasurementResponseList_t encode_trp_measurement_reponse_list(nrppa_measurement_response_list_t *in_list);
void free_nrppa_measurement_resp(nrppa_measurement_resp_t *msg);
int nrppa_gNB_measurement_failure(instance_t instance, MessageDef *msg_p);

#endif /* NRPPA_GNB_MEASUREMENT_INFORMATION_TRANSFER_H_ */
