/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdlib.h>
#include <string.h>

#include "assertions.h"
#include "nas_stream_eea1.h"
#include "snow3g.h"

void nas_stream_encrypt_eea1(nas_stream_cipher_t const *stream_cipher, uint8_t *out)
{
  uint8_t *key = (uint8_t *)stream_cipher->context;
  snow3g_ciphering(stream_cipher->count,
                   stream_cipher->bearer,
                   stream_cipher->direction,
                   key,
                   stream_cipher->blength / 8,
                   stream_cipher->message,
                   out);
}

stream_security_context_t *stream_ciphering_init_eea1(const uint8_t *ciphering_key)
{
  void *ret = calloc(1, 16);
  AssertFatal(ret != NULL, "out of memory\n");
  memcpy(ret, ciphering_key, 16);
  return (stream_security_context_t *)ret;
}

void stream_ciphering_free_eea1(stream_security_context_t *ciphering_context)
{
  free(ciphering_context);
}
