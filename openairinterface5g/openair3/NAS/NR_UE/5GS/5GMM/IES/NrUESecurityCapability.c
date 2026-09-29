/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "TLVEncoder.h"
#include "TLVDecoder.h"
#include "NrUESecurityCapability.h"

int encode_nrue_security_capability(const NrUESecurityCapability *nruesecuritycapability, uint8_t iei, uint8_t *buffer)
{
  int encoded = 0;
  if (iei) {
    *buffer = nruesecuritycapability->iei;
    encoded++;
    *(buffer + encoded) = nruesecuritycapability->length;
    encoded++;
    *(buffer + encoded) = nruesecuritycapability->fg_EA;
    encoded++;
    *(buffer + encoded) = nruesecuritycapability->fg_IA;
    encoded++;
    *(buffer + encoded) = nruesecuritycapability->EEA;
    encoded++;
    *(buffer + encoded) = nruesecuritycapability->EIA;
    encoded++;
    if ((nruesecuritycapability->length - 4) > 0)
      encoded = encoded + nruesecuritycapability->length - 4;
  }
  return encoded;
}
