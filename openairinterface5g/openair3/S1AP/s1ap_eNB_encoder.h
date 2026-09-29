/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef S1AP_ENB_ENCODER_H_
#define S1AP_ENB_ENCODER_H_

int s1ap_eNB_encode_pdu(S1AP_S1AP_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
__attribute__ ((warn_unused_result));

#endif /* S1AP_ENB_ENCODER_H_ */
