/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "aes_128_ctr.h"
#include "assertions.h"
#include "conversions.h"
#include "nas_stream_eea2.h"

void nas_stream_encrypt_eea2(nas_stream_cipher_t const *stream_cipher, uint8_t *out)
{
  DevAssert(stream_cipher != NULL);
  DevAssert(stream_cipher->bearer < 32);
  DevAssert(stream_cipher->direction < 2);
  DevAssert(stream_cipher->message != NULL);
  DevAssert(stream_cipher->blength > 7);

  aes_128_t p = {0};
  memcpy(p.key, stream_cipher->context, 16);
  p.type = AES_INITIALIZATION_VECTOR_16;
  p.iv16.d.count = htonl(stream_cipher->count);
  p.iv16.d.bearer = stream_cipher->bearer;
  p.iv16.d.direction = stream_cipher->direction;

  DevAssert((stream_cipher->blength & 0x07) == 0);
  const uint32_t byte_lenght = stream_cipher->blength >> 3;
  // Precondition: out must have enough space, at least as much as the input
  const size_t len_out = byte_lenght;
  byte_array_t msg = {.buf =  stream_cipher->message, .len = byte_lenght};
  aes_128_ctr(&p, msg, len_out, out);
}

stream_security_context_t *stream_ciphering_init_eea2(const uint8_t *ciphering_key)
{
  void *ret = calloc(1, 16);
  AssertFatal(ret != NULL, "out of memory\n");
  memcpy(ret, ciphering_key, 16);
  return (stream_security_context_t *)ret;
}

void stream_ciphering_free_eea2(stream_security_context_t *ciphering_context)
{
  free(ciphering_context);
}
