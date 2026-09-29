/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief x2ap decoder procedures for eNB
 */

#ifndef X2AP_ENB_DECODER_H_
#define X2AP_ENB_DECODER_H_

int x2ap_eNB_decode_pdu(X2AP_X2AP_PDU_t *pdu, const uint8_t *const buffer, uint32_t length)
__attribute__ ((warn_unused_result));

#endif /* X2AP_ENB_DECODER_H_ */
