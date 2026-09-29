/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief 5GS Registration Type for registration request procedures
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "FGSRegistrationType.h"

#define NO_FOLLOW_ON_REQUEST 0x0
#define FOLLOW_ON_REQUEST 0x08

int decode_5gs_registration_type(FGSRegistrationType *fgsregistrationtype, uint8_t iei, uint8_t value)
{
  int decoded = 0;
  uint8_t *buffer = &value;
  if (iei > 0) {
    CHECK_IEI_DECODER((*buffer & 0xf0), iei);
  }

  *fgsregistrationtype = *buffer & 0x7;
  decoded++;

  return decoded;
}

/**
 * @brief Encode 5GS registration type (9.11.3.7 of 3GPP TS 24.501)
 *        Note: mandatory IE (IEI = 0)
 */
int encode_5gs_registration_type(const FGSRegistrationType *fgsregistrationtype, bool follow_on_request)
{
  // Follow-on request bit
  uint8_t foR = follow_on_request ? FOLLOW_ON_REQUEST : NO_FOLLOW_ON_REQUEST;
  // Return encoded value
  return (foR | (*fgsregistrationtype & 0x7));
}
