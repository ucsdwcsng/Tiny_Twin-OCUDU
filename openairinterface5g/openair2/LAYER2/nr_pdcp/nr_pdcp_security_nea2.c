/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "openair3/SECU/aes_128_ctr.h"
#include "common/utils/assertions.h"

#include "nr_pdcp_security_nea2.h"

#include <arpa/inet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

stream_security_context_t *nr_pdcp_security_nea2_init(unsigned char *ciphering_key)
{
  // This is a hack, IMO init, cipher and free functions should be reduced to cipher.
  // Test show a ~10% more processing time
  return (stream_security_context_t *)ciphering_key;
}

void nr_pdcp_security_nea2_cipher(stream_security_context_t *security_context, unsigned char *buffer, int length, int bearer, uint32_t count, int direction)
{
  DevAssert(security_context != NULL);
  DevAssert(buffer != NULL);
  DevAssert(length > 0);
  DevAssert(bearer > -1 && bearer < 32);
  DevAssert(direction > -1 && direction < 2);

  aes_128_t p = {0};
  const uint8_t *ciphering_key = (uint8_t const *)security_context;
  memcpy(p.key, ciphering_key, 16);
  p.type = AES_INITIALIZATION_VECTOR_16;
  p.iv16.d.count = ntohl(count);
  p.iv16.d.bearer = bearer - 1;
  p.iv16.d.direction = direction;

  uint8_t out[length];
  memset(out, 0, length);

  byte_array_t msg = {.buf =  buffer, .len = length};;
  aes_128_ctr(&p, msg, length, out);

  memcpy(buffer, out, length);
}

void nr_pdcp_security_nea2_free_security(stream_security_context_t *security_context)
{
  (void)security_context;
}
