/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief x2ap encoder procedures for eNB
 */

#ifndef X2AP_ENB_ENCODER_H_
#define X2AP_ENB_ENCODER_H_

int x2ap_eNB_encode_pdu(X2AP_X2AP_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
__attribute__ ((warn_unused_result));

#endif /* X2AP_ENB_ENCODER_H_ */
