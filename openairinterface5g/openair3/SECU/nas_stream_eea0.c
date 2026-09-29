/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "nas_stream_eea0.h"

#include "common/utils/assertions.h"
#include "common/utils/LOG/log.h"

#include <string.h>

void nas_stream_encrypt_eea0(nas_stream_cipher_t const *stream_cipher, uint8_t *out)
{
  DevAssert(stream_cipher != NULL);
  DevAssert(out != NULL);

  LOG_D(OSA,
        "Entering stream_encrypt_eea0, bits length %u, bearer %u, "
        "count %u, direction %s\n",
        stream_cipher->blength,
        stream_cipher->bearer,
        stream_cipher->count,
        stream_cipher->direction == SECU_DIRECTION_DOWNLINK ? "Downlink" : "Uplink");

  uint32_t byte_length = (stream_cipher->blength + 7) >> 3;
  memcpy(out, stream_cipher->message, byte_length);
}
