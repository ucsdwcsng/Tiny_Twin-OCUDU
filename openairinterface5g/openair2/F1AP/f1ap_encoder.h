/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef F1AP_ENCODER_H_
#define F1AP_ENCODER_H_

#include "F1AP_F1AP-PDU.h"

int f1ap_encode_pdu(F1AP_F1AP_PDU_t *pdu, uint8_t **buffer, uint32_t *length)
__attribute__ ((warn_unused_result));

#endif /* F1AP_ENCODER_H_ */
