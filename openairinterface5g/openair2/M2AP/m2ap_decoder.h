/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m2ap decoder procedures for eNB
 */

#ifndef M2AP_DECODER_H_
#define M2AP_DECODER_H_

int m2ap_decode_pdu(M2AP_M2AP_PDU_t *pdu, const uint8_t *const buffer, uint32_t length)
__attribute__ ((warn_unused_result));

#endif /* M2AP_DECODER_H_ */
