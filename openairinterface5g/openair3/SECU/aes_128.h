/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef AES_128_OAI_H
#define AES_128_OAI_H 

#include <endian.h>
#include <stdint.h>
#include <stdlib.h>

// see 33.401 B.2.3 for the input to 128-EIA2
// (which is identical to 128-NIA2, see 33.501 D.3.1.3) 

typedef struct {
  uint32_t count;
  uint8_t padding: 2;
  uint8_t direction: 1;
  uint8_t bearer: 5;
  uint8_t pad_8;
  uint16_t pad_16;
  uint64_t pad_64;
} data_iv16_t;

typedef struct{
 union{
   data_iv16_t d;
   uint8_t iv[16];
 }; 
} iv16_t;

typedef struct {
  uint32_t count;
  uint8_t padding: 2;
  uint8_t direction: 1;
  uint8_t bearer: 5;
  uint8_t pad_8;
  uint16_t pad_16;
} data_iv8_t;

typedef struct{
 union{
   data_iv8_t d;
   uint8_t iv[8];
 }; 
} iv8_t;

typedef enum {
  AES_INITIALIZATION_VECTOR_8,
  AES_INITIALIZATION_VECTOR_16,
  NONE_INITIALIZATION_VECTOR,

  AES_INITIALIZATION_VECTOR_END,
} iv_e;

#if __BYTE_ORDER == __BIG_ENDIAN
static_assert(0 != 0, "The struct iv_t needs little endianness");
#endif

// static_assert((16 == sizeof(iv_t)) , "Initialization Vector needs to have an AES_BLOCK_SIZE" );

typedef struct {
  uint8_t key[16];
  iv_e type;
  union{
    iv8_t iv8;
    iv16_t iv16;
  };
} aes_128_t;

#endif

