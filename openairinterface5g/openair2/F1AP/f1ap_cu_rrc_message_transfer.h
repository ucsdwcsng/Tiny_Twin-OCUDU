/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef F1AP_CU_RRC_MESSAGE_TRANSFER_H_
#define F1AP_CU_RRC_MESSAGE_TRANSFER_H_

int CU_handle_INITIAL_UL_RRC_MESSAGE_TRANSFER(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

int CU_send_DL_RRC_MESSAGE_TRANSFER(sctp_assoc_t assoc_id, f1ap_dl_rrc_message_t *f1ap_dl_rrc);

int CU_handle_UL_RRC_MESSAGE_TRANSFER(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, F1AP_F1AP_PDU_t *pdu);

#endif /* F1AP_CU_RRC_MESSAGE_TRANSFER_H_ */
