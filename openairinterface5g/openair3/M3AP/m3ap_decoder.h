/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m3ap decoder procedures for eNB
 */

#ifndef M3AP_DECODER_H_
#define M3AP_DECODER_H_

int m3ap_decode_pdu(M3AP_M3AP_PDU_t *pdu, const uint8_t *const buffer, uint32_t length)
__attribute__ ((warn_unused_result));

#endif /* M3AP_DECODER_H_ */
