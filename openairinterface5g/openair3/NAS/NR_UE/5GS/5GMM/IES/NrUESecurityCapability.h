/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef NRUE_SECURITY_CAPABILITY_H_
#define NRUE_SECURITY_CAPABILITY_H_

typedef struct {
  uint8_t iei;
  uint8_t length;
  uint8_t fg_EA;
  uint8_t fg_IA;
  uint8_t EEA;
  uint8_t EIA;
} NrUESecurityCapability;

int encode_nrue_security_capability(const NrUESecurityCapability *nruesecuritycapability, uint8_t iei, uint8_t *buffer);

#endif /* NRUE_SECURITY_CAPABILITY_H_ */
