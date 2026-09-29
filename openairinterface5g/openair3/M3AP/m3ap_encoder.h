/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m3ap encoder procedures
 */

#ifndef M3AP_ENCODER_H_
#define M3AP_ENCODER_H_

int m3ap_encode_pdu(M3AP_M3AP_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
__attribute__ ((warn_unused_result));

#endif /* M3AP_ENCODER_H_ */
