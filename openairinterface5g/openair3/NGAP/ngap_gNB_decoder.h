/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */
/*!
 * \brief ngap pdu decode procedures for gNB
 */

#include <stdint.h>
#include "NGAP_NGAP-PDU.h"

#ifndef NGAP_GNB_DECODER_H_
#define NGAP_GNB_DECODER_H_

int ngap_gNB_decode_pdu(NGAP_NGAP_PDU_t *pdu, const uint8_t *const buffer,
                        const uint32_t length) __attribute__ ((warn_unused_result));

#endif /* NGAP_GNB_DECODER_H_ */
