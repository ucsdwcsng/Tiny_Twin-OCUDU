/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */


#ifndef AES_128_CBC_CMAC_H
#define AES_128_CBC_CMAC_H 

#include "aes_128.h"
#include "common/utils/ds/byte_array.h"


#include <stdint.h>
#include <stdlib.h>

typedef struct {
  void *mac_implementation;
  void *mac_context;
  uint8_t key[16];
} cbc_cmac_ctx_t ;

cbc_cmac_ctx_t init_aes_128_cbc_cmac(const uint8_t key[16]);
void cipher_aes_128_cbc_cmac(cbc_cmac_ctx_t const* ctx, const aes_128_t* k_iv, byte_array_t msg, size_t len_out, uint8_t out[len_out]);
void free_aes_128_cbc_cmac(cbc_cmac_ctx_t const* ctx);

#endif

