/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief pdu session establishment request procedures
 */

#include "PduSessionEstablishRequest.h"
#include <stdint.h>
#include "TLVEncoder.h"

int encode_pdu_session_establishment_request(pdu_session_establishment_request_msg *pdusessionestablishrequest, uint8_t *buffer)
{
  int encoded = 0;

  *(buffer + encoded) = pdusessionestablishrequest->protocoldiscriminator;
  encoded++;
  *(buffer + encoded) = pdusessionestablishrequest->pdusessionid;
  encoded++;
  *(buffer + encoded) = pdusessionestablishrequest->pti;
  encoded++;
  *(buffer + encoded) = pdusessionestablishrequest->pdusessionestblishmsgtype;
  encoded++;

  IES_ENCODE_U16(buffer, encoded, pdusessionestablishrequest->maxdatarate);
  *(buffer + encoded) = pdusessionestablishrequest->pdusessiontype;
  encoded++;

  return encoded;
}
