/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef F1AP_DU_UE_CONTEXT_MANAGEMENT_H_
#define F1AP_DU_UE_CONTEXT_MANAGEMENT_H_

/*
 * UE Context Setup
 */
int DU_send_UE_CONTEXT_SETUP_RESPONSE(sctp_assoc_t assoc_id, f1ap_ue_context_setup_resp_t *resp);

int DU_handle_UE_CONTEXT_SETUP_REQUEST(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);
int DU_send_UE_CONTEXT_SETUP_FAILURE(sctp_assoc_t assoc_id);


/*
 * UE Context Release Request (gNB-DU initiated)
 */
int DU_send_UE_CONTEXT_RELEASE_REQUEST(sctp_assoc_t assoc_id, f1ap_ue_context_rel_req_t *req);

/*
 * UE Context Release Command (gNB-CU initiated)
 */
int DU_handle_UE_CONTEXT_RELEASE_COMMAND(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

/*
 * UE Context Release Complete (gNB-DU initiated)
 */
int DU_send_UE_CONTEXT_RELEASE_COMPLETE(sctp_assoc_t assoc_id, f1ap_ue_context_rel_cplt_t *cplt);

/*
 * UE Context Modification (gNB-CU initiated)
 */
int DU_handle_UE_CONTEXT_MODIFICATION_REQUEST(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);
int DU_send_UE_CONTEXT_MODIFICATION_RESPONSE(sctp_assoc_t assoc_id, f1ap_ue_context_mod_resp_t *resp);
int DU_send_UE_CONTEXT_MODIFICATION_FAILURE(sctp_assoc_t assoc_id);


/*
 * UE Context Modification Required (gNB-DU initiated)
 */
int DU_send_UE_CONTEXT_MODIFICATION_REQUIRED(sctp_assoc_t assoc_id, f1ap_ue_context_modif_required_t *required);
int DU_handle_UE_CONTEXT_MODIFICATION_CONFIRM(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);
int DU_handle_UE_CONTEXT_MODIFICATION_REFUSE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

/*
 * UE Inactivity Notification
 */

/*
 * Notify
 */

#endif /* F1AP_DU_UE_CONTEXT_MANAGEMENT_H_ */
