/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef UE_SECURITY_CAPABILITY_H_
#define UE_SECURITY_CAPABILITY_H_

#define UE_SECURITY_CAPABILITY_MINIMUM_LENGTH 2
#define UE_SECURITY_CAPABILITY_MAXIMUM_LENGTH 7

typedef struct UeSecurityCapability_tag {
  /* EPS encryption algorithms supported (octet 3) */
  uint8_t  eea;
  /* EPS integrity algorithms supported (octet 4) */
  uint8_t  eia;
  uint8_t  umts_present;
  uint8_t  gprs_present;
  /* UMTS encryption algorithms supported (octet 5) */
  uint8_t  uea;
  /* UMTS integrity algorithms supported (octet 6) */
  uint8_t  uia:7;
  /* GPRS encryption algorithms supported (octet 7) */
  uint8_t  gea:7;
} UeSecurityCapability;

int encode_ue_security_capability(UeSecurityCapability *uesecuritycapability, uint8_t iei, uint8_t *buffer, uint32_t len);

int decode_ue_security_capability(UeSecurityCapability *uesecuritycapability, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_ue_security_capability_xml(UeSecurityCapability *uesecuritycapability, uint8_t iei);

#endif /* UE SECURITY CAPABILITY_H_ */

