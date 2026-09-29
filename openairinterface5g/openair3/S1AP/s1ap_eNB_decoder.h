/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdint.h>

#ifndef S1AP_ENB_DECODER_H_
#define S1AP_ENB_DECODER_H_

int s1ap_eNB_decode_pdu(S1AP_S1AP_PDU_t *pdu, const uint8_t *const buffer,
                        const uint32_t length) __attribute__ ((warn_unused_result));

#endif /* S1AP_ENB_DECODER_H_ */
