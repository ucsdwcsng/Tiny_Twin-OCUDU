/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NRPPA_GNB_ENCODER_H_
#define NRPPA_GNB_ENCODER_H_

int nrppa_gNB_encode_pdu(NRPPA_NRPPA_PDU_t *pdu, uint8_t **buffer, uint32_t *len) __attribute__((warn_unused_result));

#endif /* NRPPA_GNB_ENCODER_H_ */
