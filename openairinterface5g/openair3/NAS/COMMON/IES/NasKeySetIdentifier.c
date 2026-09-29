/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "NasKeySetIdentifier.h"

/**
 * @brief Decode the NAS key set identifier from one byte
 */
int decode_nas_key_set_identifier(NasKeySetIdentifier *naskeysetidentifier, uint8_t iei, uint8_t value)
{
  if (iei > 0) {
    CHECK_IEI_DECODER((value & 0xf0), iei);
  }

  naskeysetidentifier->tsc = (value >> 3) & 0x1;
  naskeysetidentifier->naskeysetidentifier = value & 0x7;

#if defined(NAS_DEBUG)
  dump_nas_key_set_identifier_xml(naskeysetidentifier, iei);
#endif

  return 1; // 1 octet
}

/**
 * @brief Encode the NAS key set identifier in one byte
 */
uint8_t encode_nas_key_set_identifier(const NasKeySetIdentifier *naskeysetidentifier, uint8_t iei)
{
#if defined (NAS_DEBUG)
  dump_nas_key_set_identifier_xml(naskeysetidentifier, iei);
#endif
  return ((iei & 0xf0) | (naskeysetidentifier->tsc & 0x1) << 3) | (naskeysetidentifier->naskeysetidentifier & 0x7);
}

void dump_nas_key_set_identifier_xml(NasKeySetIdentifier *naskeysetidentifier, uint8_t iei)
{
  printf("<Nas Key Set Identifier>\n");

  if (iei > 0)
    /* Don't display IEI if = 0 */
    printf("    <IEI>0x%X</IEI>\n", iei);

  printf("    <TSC>%u</TSC>\n", naskeysetidentifier->tsc);
  printf("    <NAS key set identifier>%u</NAS key set identifier>\n", naskeysetidentifier->naskeysetidentifier);
  printf("</Nas Key Set Identifier>\n");
}

