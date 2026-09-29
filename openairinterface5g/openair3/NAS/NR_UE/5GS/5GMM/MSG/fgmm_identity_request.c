/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "fgmm_identity_request.h"
#include <string.h>
#include "FGSMobileIdentity.h"
#include "common/utils/eq_check.h"

int decode_fgs_identity_request(fgs_identity_request_msg_t *msg, const byte_array_t *buffer)
{
  if (buffer->len < 1) {
    PRINT_ERROR("Failed to decode NAS Identity Request: invalid buffer length %ld\n", buffer->len);
    return -1;
  }

  // Bits 3-1 (least significant 3 bits)
  msg->fgsmobileidentity = buffer->buf[0] & 0x07;

  return 1;
}
