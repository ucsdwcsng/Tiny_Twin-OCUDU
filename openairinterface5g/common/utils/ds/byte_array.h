/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef BYTE_ARRAY_H 
#define BYTE_ARRAY_H 

/* WARNING: This file is also defined at XXXXX. Both files need to be completely equal. Same applies for *.c */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define FREE_AND_ZERO_BYTE_ARRAY(BA) \
  do {                               \
    free_byte_array(BA);             \
    BA.buf = NULL;                   \
    BA.len = 0;                      \
  } while (0)

typedef struct {
  size_t len;
  uint8_t* buf;
} byte_array_t;

typedef struct {
  uint8_t buf[32];
} byte_array_32_t;

byte_array_t copy_byte_array(byte_array_t src);
void free_byte_array(byte_array_t ba);
bool eq_byte_array(const byte_array_t* m0, const byte_array_t* m1);

byte_array_t cp_str_to_ba(const char* str);
char* cp_ba_to_str(const byte_array_t ba);
byte_array_t create_byte_array(const size_t len, const uint8_t* buffer);

#endif
