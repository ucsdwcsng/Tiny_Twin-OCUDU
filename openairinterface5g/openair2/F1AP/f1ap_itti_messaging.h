/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef F1AP_ITTI_MESSAGING_H_
#define F1AP_ITTI_MESSAGING_H_

void f1ap_itti_send_sctp_data_req(sctp_assoc_t assoc_id, uint8_t *buffer, uint32_t buffer_length);

#endif /* F1AP_ITTI_MESSAGING_H_ */
