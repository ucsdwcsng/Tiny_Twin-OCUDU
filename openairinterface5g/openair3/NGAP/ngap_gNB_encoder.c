/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief ngap pdu encode procedures for gNB
 */

#include "ngap_gNB_encoder.h"
#include <stdint.h>
#include <stdio.h>
#include "ngap_msg_includes.h"
#include "T.h"
#include "aper_encoder.h"
#include "asn_application.h"
#include "assertions.h"
#include "common/utils/T/T.h"
#include "ngap_common.h"
#include "xer_encoder.h"

int ngap_gNB_encode_pdu(NGAP_NGAP_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
{
  DevAssert(pdu != NULL);
  DevAssert(buffer != NULL);
  DevAssert(len != NULL);

  if (LOG_DEBUGFLAG(DEBUG_ASN1)) {
    xer_fprint(stdout, &asn_DEF_NGAP_NGAP_PDU, pdu);
  }

  char errbuf[256];
  size_t errlen = sizeof(errbuf);
  if (asn_check_constraints(&asn_DEF_NGAP_NGAP_PDU, pdu, errbuf, &errlen)) {
    xer_fprint(stdout, &asn_DEF_NGAP_NGAP_PDU, pdu);
    NGAP_ERROR("Constraint validation failed: %s\n", errbuf);
    return -1;
  }

  void *buf = NULL;
  ssize_t encoded = aper_encode_to_new_buffer(&asn_DEF_NGAP_NGAP_PDU, NULL, pdu, &buf);
  if (encoded < 0) {
    NGAP_ERROR("Failed to encode NGAP PDU\n");
    return -1;
  }

  *buffer = buf;
  *len = encoded;
  return 0;
}
