/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef BYTE_ARRAY_PRODUCER_H
#define BYTE_ARRAY_PRODUCER_H

#include <stdint.h>

#include "byte_array.h"

typedef struct {
  byte_array_t b;
  size_t pos;
} byte_array_producer_t;

byte_array_producer_t byte_array_producer_from_buffer(uint8_t *buffer, int len);

int byte_array_producer_put_byte(byte_array_producer_t *b, uint8_t v);
int byte_array_producer_put_u32_be(byte_array_producer_t *b, uint32_t v);
int byte_array_producer_put_u24_be(byte_array_producer_t *b, uint32_t v);

#endif
