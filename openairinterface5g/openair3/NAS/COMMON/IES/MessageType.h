/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef MESSAGE_TYPE_H_
#define MESSAGE_TYPE_H_

typedef uint8_t MessageType;

int encode_message_type(MessageType *messagetype, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_message_type_xml(MessageType *messagetype, uint8_t iei);

int decode_message_type(MessageType *messagetype, uint8_t iei, uint8_t *buffer, uint32_t len);

#endif /* MESSAGE TYPE_H_ */

