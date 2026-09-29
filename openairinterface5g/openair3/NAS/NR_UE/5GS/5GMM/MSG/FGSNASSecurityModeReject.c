/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdint.h>
#include "FGSNASSecurityModeReject.h"
#include "common/utils/ds/byte_array.h"
#include "common/utils/eq_check.h"
#include "fgmm_lib.h"

int encode_fgs_security_mode_reject(byte_array_t *buffer, const fgs_security_mode_reject_msg *msg)
{
  if (buffer->len < 1)
    return -1; // nothing to encode

  // 5GMM cause (Mandatory)
  return encode_fgs_nas_cause(buffer, &msg->cause);
}

/** @brief Decode Security Mode Reject (8.2.18 of 3GPP TS 24.501) */
int decode_fgs_security_mode_reject(fgs_security_mode_reject_msg *msg, const byte_array_t *buffer)
{
  if (buffer->len < 1)
    return -1; // nothing to decode

  // 5GMM cause (Mandatory)
  return decode_fgs_nas_cause(&msg->cause, buffer);
}

bool eq_sec_mode_reject(const fgs_security_mode_reject_msg *a, const fgs_security_mode_reject_msg *b)
{
  _EQ_CHECK_INT(a->cause, b->cause);
  return true;
}
