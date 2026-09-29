/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*! \file f1ap_encoder.c
 * \brief f1ap pdu encode procedures
 */

#include "f1ap_common.h"
#include "f1ap_encoder.h"

static int asn1_encoder_xer_print = 0;

int f1ap_encode_pdu(F1AP_F1AP_PDU_t *pdu, uint8_t **buffer, uint32_t *length) {
  ssize_t    encoded;
  DevAssert(pdu != NULL);
  DevAssert(buffer != NULL);
  DevAssert(length != NULL);

  if (asn1_encoder_xer_print) {
    LOG_E(F1AP, "----------------- ASN1 ENCODER PRINT START ----------------- \n");
    xer_fprint(stdout, &asn_DEF_F1AP_F1AP_PDU, pdu);
    LOG_E(F1AP, "----------------- ASN1 ENCODER PRINT END----------------- \n");
  }

  char errbuf[4096]; /* Buffer for error message */
  size_t errlen = sizeof(errbuf); /* Size of the buffer */
  int ret = asn_check_constraints(&asn_DEF_F1AP_F1AP_PDU, pdu, errbuf, &errlen);

  if(ret) {
    xer_fprint(stdout, &asn_DEF_F1AP_F1AP_PDU, pdu);
    LOG_E(F1AP, "Constraint validation failed: %s\n", errbuf);
  }

  encoded = aper_encode_to_new_buffer(&asn_DEF_F1AP_F1AP_PDU, 0, pdu, (void **)buffer);

  if (encoded < 0) {
    LOG_E(F1AP, "Failed to encode F1AP message\n");
    return -1;
  }

  *length = encoded;
  return encoded;
}
