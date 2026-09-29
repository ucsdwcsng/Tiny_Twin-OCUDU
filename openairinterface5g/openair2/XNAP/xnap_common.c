/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdint.h>
#include "xnap_common.h"
#include "XNAP_XnAP-PDU.h"

ssize_t XNAP_generate_initiating_message(uint8_t **buffer,
                                         uint32_t *length,
                                         XNAP_ProcedureCode_t procedureCode,
                                         XNAP_Criticality_t criticality,
                                         asn_TYPE_descriptor_t *td,
                                         void *sptr)
{
  XNAP_XnAP_PDU_t pdu;
  ssize_t encoded;
  memset(&pdu, 0, sizeof(XNAP_XnAP_PDU_t));
  pdu.present = XNAP_XnAP_PDU_PR_initiatingMessage;
  pdu.choice.initiatingMessage->procedureCode = procedureCode;
  pdu.choice.initiatingMessage->criticality = criticality;
  ANY_fromType_aper((ANY_t *)&pdu.choice.initiatingMessage->value, td, sptr);

  if ((encoded = aper_encode_to_new_buffer(&asn_DEF_XNAP_XnAP_PDU, 0, &pdu, (void **)buffer)) < 0) {
    return -1;
  }

  *length = encoded;
  return encoded;
}

