/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief 5GS Mobile Identity for registration request procedures
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "FGMMCapability.h"

#define MIN_LENGTH_5GMM_CAPABILITY 3

/**
 * @brief Encode 5GMM Capability IE (9.11.3 of 3GPP TS 24.501)
 */
int encode_5gmm_capability(uint8_t *buffer, const FGMMCapability *fgmmcapability, uint32_t len)
{
  int encoded = 0;

  // Ensure buffer length is sufficient
  if (len < MIN_LENGTH_5GMM_CAPABILITY) {
    printf("Buffer length insufficient for encoding 5GMM capability\n");
    return -1;
  }

  if (fgmmcapability->iei) {
    *buffer = fgmmcapability->iei;
    encoded++;
  } else {
    printf("Missing IEI in fgmmcapability\n");
  }

  buffer[encoded] = fgmmcapability->length;
  encoded++;

  // Encode octet 3 (mandatory)
  buffer[encoded] = (fgmmcapability->sgc << 7) | (fgmmcapability->iphc_cp_cIoT << 6) | (fgmmcapability->n3_data << 5)
                    | (fgmmcapability->cp_cIoT << 4) | (fgmmcapability->restrict_ec << 3) | (fgmmcapability->lpp << 2)
                    | (fgmmcapability->ho_attach << 1) | fgmmcapability->s1_mode;
  encoded++;

  // Encode octet 4
  if (fgmmcapability->length >= 2) {
    buffer[encoded] = (fgmmcapability->racs << 7) | (fgmmcapability->nssaa << 6) | (fgmmcapability->lcs << 5)
                      | (fgmmcapability->v2x_cnpc5 << 4) | (fgmmcapability->v2x_cepc5 << 3) | (fgmmcapability->v2x << 2)
                      | (fgmmcapability->up_cIoT << 1) | fgmmcapability->srvcc;
    encoded++;
  }

  // Encode octet 5
  if (fgmmcapability->length >= 3) {
    buffer[encoded] = (fgmmcapability->ehc_CP_ciot << 3) | (fgmmcapability->multiple_eUP << 2) | (fgmmcapability->wusa << 1)
                      | fgmmcapability->cag;
    encoded++;
  }

  // Encode octets 6 - 15
  if (fgmmcapability->length >= 4)
    memset(&buffer[encoded], 0, fgmmcapability->length - 3);

  return encoded;
}
