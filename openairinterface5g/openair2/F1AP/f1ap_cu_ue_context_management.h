/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef F1AP_CU_UE_CONTEXT_MANAGEMENT_H_
#define F1AP_CU_UE_CONTEXT_MANAGEMENT_H_

/*
 * UE Context Setup
 */
int CU_send_UE_CONTEXT_SETUP_REQUEST(sctp_assoc_t assoc_id, const f1ap_ue_context_setup_req_t *req);

int CU_handle_UE_CONTEXT_SETUP_RESPONSE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

int CU_handle_UE_CONTEXT_SETUP_FAILURE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

/*
 * UE Context Release Request (gNB-DU initiated)
 */
int CU_handle_UE_CONTEXT_RELEASE_REQUEST(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

/*
 * UE Context Release (gNB-CU initiated)
 */
int CU_send_UE_CONTEXT_RELEASE_COMMAND(sctp_assoc_t assoc_id, f1ap_ue_context_rel_cmd_t *cmd);

int CU_handle_UE_CONTEXT_RELEASE_COMPLETE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

/*
 * UE Context Modification (gNB-CU initiated)
 */
int CU_send_UE_CONTEXT_MODIFICATION_REQUEST(sctp_assoc_t assoc_id, const f1ap_ue_context_mod_req_t *req);
int CU_handle_UE_CONTEXT_MODIFICATION_RESPONSE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);
int CU_handle_UE_CONTEXT_MODIFICATION_FAILURE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

/*
 * UE Context Modification Required (gNB-DU initiated)
 */
int CU_handle_UE_CONTEXT_MODIFICATION_REQUIRED(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

int CU_send_UE_CONTEXT_MODIFICATION_CONFIRM(sctp_assoc_t assoc_id, f1ap_ue_context_modif_confirm_t *confirm);

int CU_send_UE_CONTEXT_MODIFICATION_REFUSE(sctp_assoc_t assoc_id, f1ap_ue_context_modif_refuse_t *refuse);

/*
 * UE Inactivity Notification
 */

/*
 * Notify
 */

#endif /* F1AP_CU_UE_CONTEXT_MANAGEMENT_H_ */
