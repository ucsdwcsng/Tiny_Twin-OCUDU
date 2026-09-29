/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "nrppa_common.h"

#ifndef NRPPA_GNB_DECODER_H_
#define NRPPA_GNB_DECODER_H_

int nrppa_gNB_decode_pdu(NRPPA_NRPPA_PDU_t *pdu, const uint8_t *const buffer, const uint32_t length)
    __attribute__((warn_unused_result));

#endif /* NRPPA_GNB_DECODER_H_ */
