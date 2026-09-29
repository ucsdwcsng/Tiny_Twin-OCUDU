/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */


#ifndef F1AP_DU_RRC_MESSAGE_TRANSFER_H_
#define F1AP_DU_RRC_MESSAGE_TRANSFER_H_

#include <openair2/RRC/NR/MESSAGES/asn1_msg.h>

struct F1AP_F1AP_PDU;
int DU_handle_DL_RRC_MESSAGE_TRANSFER(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, struct F1AP_F1AP_PDU *pdu);

int DU_send_UL_NR_RRC_MESSAGE_TRANSFER(sctp_assoc_t assoc_id, const f1ap_ul_rrc_message_t *msg);

int DU_send_INITIAL_UL_RRC_MESSAGE_TRANSFER(sctp_assoc_t assoc_id, const f1ap_initial_ul_rrc_message_t *msg);

#endif /* F1AP_DU_RRC_MESSAGE_TRANSFER_H_ */
