/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "nas_stream_eia2.h"

#include <arpa/inet.h>
#include "aes_128_cbc_cmac.h"
#include "assertions.h"
#include "conversions.h"

/*!
 * @brief Create integrity cmac t for a given message.
 * @param[in] stream_cipher Structure containing various variables to setup encoding
 * @param[out] out For EIA2 the output string is 32 bits long
 */

void nas_stream_encrypt_eia2(nas_stream_cipher_t const *stream_cipher, uint8_t out[4])
{
  DevAssert(stream_cipher != NULL);
  DevAssert(stream_cipher->message != NULL);
  DevAssert(stream_cipher->bearer < 32);
  DevAssert((stream_cipher->blength & 0x7) == 0); 

  cbc_cmac_ctx_t *ctx = (cbc_cmac_ctx_t *)stream_cipher->context;
  aes_128_t k_iv = {0};
  memcpy(&k_iv.key, ctx->key, 16);
  k_iv.type = AES_INITIALIZATION_VECTOR_8;
  k_iv.iv8.d.bearer = stream_cipher->bearer;
  k_iv.iv8.d.direction = stream_cipher->direction;
  k_iv.iv8.d.count = htonl(stream_cipher->count);

  size_t const m_length = stream_cipher->blength >> 3;
  uint8_t result[16] = {0};
  byte_array_t msg = {.buf = stream_cipher->message, .len = m_length };
  cipher_aes_128_cbc_cmac(ctx, &k_iv, msg, sizeof(result), result);

  memcpy(out, result, 4);
}

stream_security_context_t *stream_integrity_init_eia2(const uint8_t *integrity_key)
{
  cbc_cmac_ctx_t *ret = calloc(1, sizeof(cbc_cmac_ctx_t));
  AssertFatal(ret != NULL, "out of memory\n");
  *ret = init_aes_128_cbc_cmac(integrity_key);
  return (stream_security_context_t *)ret;
}

void stream_integrity_free_eia2(stream_security_context_t *integrity_context)
{
  free_aes_128_cbc_cmac((cbc_cmac_ctx_t *)integrity_context);
  free(integrity_context);
}
