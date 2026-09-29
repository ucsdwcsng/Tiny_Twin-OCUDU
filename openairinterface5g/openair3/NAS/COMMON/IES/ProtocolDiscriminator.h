/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef PROTOCOL_DISCRIMINATOR_H_
#define PROTOCOL_DISCRIMINATOR_H_

typedef uint8_t ProtocolDiscriminator;

int encode_protocol_discriminator(ProtocolDiscriminator *protocoldiscriminator, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_protocol_discriminator_xml(ProtocolDiscriminator *protocoldiscriminator, uint8_t iei);

int decode_protocol_discriminator(ProtocolDiscriminator *protocoldiscriminator, uint8_t iei, uint8_t *buffer, uint32_t len);

#endif /* PROTOCOL DISCRIMINATOR_H_ */

