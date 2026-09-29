/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef F1AP_CU_INTERFACE_MANAGEMENT_H_
#define F1AP_CU_INTERFACE_MANAGEMENT_H_

#include "openair2/COMMON/f1ap_messages_types.h"

struct F1AP_F1AP_PDU;
typedef struct F1AP_F1AP_PDU F1AP_F1AP_PDU_t;

/*
 * Reset
 */
void CU_send_RESET(sctp_assoc_t assoc_id, const f1ap_reset_t *reset);
int CU_handle_RESET_ACKNOWLEDGE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);
int CU_send_RESET_ACKNOWLEDGE(sctp_assoc_t assoc_id, const f1ap_reset_ack_t *ack);

/*
 * F1 Setup
 */
int CU_handle_F1_SETUP_REQUEST(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

int CU_send_F1_SETUP_RESPONSE(sctp_assoc_t assoc_id, f1ap_setup_resp_t *f1ap_setup_resp);

int CU_send_F1_SETUP_FAILURE(sctp_assoc_t assoc_id, const f1ap_setup_failure_t *fail);

/*
 * gNB-DU Configuration Update
 */
int CU_handle_gNB_DU_CONFIGURATION_UPDATE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

int CU_send_gNB_DU_CONFIGURATION_UPDATE_ACKNOWLEDGE(
    sctp_assoc_t assoc_id,
    f1ap_gnb_du_configuration_update_acknowledge_t *GNBDUConfigurationUpdateAcknowledge);

/*
 * gNB-CU Configuration Update
 */
int CU_send_gNB_CU_CONFIGURATION_UPDATE(sctp_assoc_t assoc_id, f1ap_gnb_cu_configuration_update_t *f1ap_gnb_cu_configuration_update);

int CU_handle_gNB_CU_CONFIGURATION_UPDATE_ACKNOWLEDGE(instance_t instance,
                                                      sctp_assoc_t assoc_id,
                                                      uint32_t stream,
                                                      F1AP_F1AP_PDU_t *pdu);

#endif /* F1AP_CU_INTERFACE_MANAGEMENT_H_ */
