/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "nr_pdcp_security_nea1.h"

#include <stdlib.h>
#include <string.h>

#include "assertions.h"

stream_security_context_t *nr_pdcp_security_nea1_init(unsigned char *ciphering_key)
{
  nas_stream_cipher_t *ret;

  ret = calloc(1, sizeof(*ret));
  AssertFatal(ret != NULL, "out of memory\n");
  ret->context = malloc(16);
  AssertFatal(ret->context != NULL, "out of memory\n");
  memcpy(ret->context, ciphering_key, 16);

  return (stream_security_context_t *)ret;
}

void nr_pdcp_security_nea1_cipher(stream_security_context_t *security_context,
                                  unsigned char *buffer,
                                  int length,
                                  int bearer,
                                  uint32_t count,
                                  int direction)
{
  nas_stream_cipher_t *ctx = (nas_stream_cipher_t *)security_context;

  ctx->message = buffer;
  ctx->count = count;
  ctx->bearer = bearer - 1;
  ctx->direction = direction;
  ctx->blength = length * 8;

  uint8_t out[length];

  stream_compute_encrypt(EEA1_128_ALG_ID, ctx, out);

  memcpy(buffer, out, length);
}

void nr_pdcp_security_nea1_free_security(stream_security_context_t *security_context)
{
  nas_stream_cipher_t *ctx = (nas_stream_cipher_t *)security_context;
  free(ctx->context);
  free(ctx);
}
