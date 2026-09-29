/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief ngap pdu encode procedures for gNB
 */
 
#ifndef NGAP_GNB_ENCODER_H_
#define NGAP_GNB_ENCODER_H_

#include <stdint.h>
#include "NGAP_NGAP-PDU.h"

int ngap_gNB_encode_pdu(NGAP_NGAP_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
__attribute__ ((warn_unused_result));

#endif /* NGAP_GNB_ENCODER_H_ */
