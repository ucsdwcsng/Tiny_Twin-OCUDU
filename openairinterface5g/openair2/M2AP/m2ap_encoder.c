/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m2ap encoder procedures
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "assertions.h"
#include "conversions.h"
#include "intertask_interface.h"
#include "m2ap_common.h"
#include "m2ap_encoder.h"

int m2ap_encode_pdu(M2AP_M2AP_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
{
  ssize_t    encoded;

  DevAssert(pdu != NULL);
  DevAssert(buffer != NULL);
  DevAssert(len != NULL);

  if (LOG_DEBUGFLAG(DEBUG_ASN1)) {
    xer_fprint(stdout, &asn_DEF_M2AP_M2AP_PDU, (void *)pdu);
  }

  encoded = aper_encode_to_new_buffer(&asn_DEF_M2AP_M2AP_PDU, 0, pdu, (void **)buffer);

  if (encoded < 0) {
    return -1;
  }

  *len = encoded;

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_M2AP_M2AP_PDU, pdu);
  return encoded;
}
