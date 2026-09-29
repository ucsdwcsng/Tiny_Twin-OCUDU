/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef NAS_KEY_SET_IDENTIFIER_H_
#define NAS_KEY_SET_IDENTIFIER_H_

#define NAS_KEY_SET_IDENTIFIER_MINIMUM_LENGTH 1
#define NAS_KEY_SET_IDENTIFIER_MAXIMUM_LENGTH 1

typedef struct NasKeySetIdentifier_tag {
#define NAS_KEY_SET_IDENTIFIER_NATIVE 0
#define NAS_KEY_SET_IDENTIFIER_MAPPED 1
  uint8_t  tsc:1;
#define NAS_KEY_SET_IDENTIFIER_NOT_AVAILABLE 0b111
  uint8_t  naskeysetidentifier:3;
} NasKeySetIdentifier;

uint8_t encode_nas_key_set_identifier(const NasKeySetIdentifier *naskeysetidentifier, uint8_t iei);

void dump_nas_key_set_identifier_xml(NasKeySetIdentifier *naskeysetidentifier, uint8_t iei);

int decode_nas_key_set_identifier(NasKeySetIdentifier *naskeysetidentifier, uint8_t iei, uint8_t value);

#endif /* NAS KEY SET IDENTIFIER_H_ */

