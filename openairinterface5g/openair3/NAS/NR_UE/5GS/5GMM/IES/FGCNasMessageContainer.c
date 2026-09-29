/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief security mode complete procedures for gNB
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "FGCNasMessageContainer.h"

int decode_fgc_nas_message_container(FGCNasMessageContainer *nasmessagecontainer, uint8_t iei, uint8_t *buffer, uint32_t len)
{
  int decoded = 0;
  uint16_t ielen = 0;
  int decode_result;

  if (iei > 0) {
    CHECK_IEI_DECODER(iei, *buffer);
    decoded++;
  }

  uint16_t encoded_ielen;
  memcpy(&encoded_ielen, buffer + decoded, sizeof(encoded_ielen));
  ielen = ntohs(encoded_ielen);
  decoded += sizeof(encoded_ielen);
  CHECK_LENGTH_DECODER(len - decoded, ielen);

  if ((decode_result =
           decode_octet_string(&nasmessagecontainer->nasmessagecontainercontents, ielen, buffer + decoded, len - decoded))
      < 0)
    return decode_result;
  else
    decoded += decode_result;

  return decoded;
}

int encode_fgc_nas_message_container(const FGCNasMessageContainer *nasmessagecontainer, uint8_t iei, uint8_t *buffer, uint32_t len)
{
  uint32_t encoded = 0;
  int encode_result;
  /* Checking IEI and pointer */
  CHECK_PDU_POINTER_AND_LENGTH_ENCODER(buffer, FGC_NAS_MESSAGE_CONTAINER_MINIMUM_LENGTH, len);

  if (iei > 0) {
    *buffer = iei;
    encoded++;
  }

  encoded += 2;

  if ((encode_result = encode_octet_string(&nasmessagecontainer->nasmessagecontainercontents, buffer + encoded, len - encoded))
      < 0) {
    return encode_result;
  } else {
    uint16_t tmp = htons(encoded + encode_result - 3);
    memcpy(buffer + 1, &tmp, sizeof(tmp));
    encoded += encode_result;
  }

  return encoded;
}

void dump_fgc_nas_message_container_xml(FGCNasMessageContainer *nasmessagecontainer, uint8_t iei)
{
  printf("<Nas Message Container>\n");

  if (iei > 0)
    /* Don't display IEI if = 0 */
    printf("    <IEI>0x%X</IEI>\n", iei);

  printf("%s", dump_octet_string_xml(&nasmessagecontainer->nasmessagecontainercontents));
  printf("</Nas Message Container>\n");
}
