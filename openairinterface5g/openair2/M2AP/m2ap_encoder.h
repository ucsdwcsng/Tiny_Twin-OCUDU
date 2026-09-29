/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m2ap encoder procedures
 */

#ifndef M2AP_ENCODER_H_
#define M2AP_ENCODER_H_

int m2ap_encode_pdu(M2AP_M2AP_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
__attribute__ ((warn_unused_result));

#endif /* M2AP_ENCODER_H_ */
