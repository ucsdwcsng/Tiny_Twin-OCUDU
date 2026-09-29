/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "fgmm_registration_reject.h"
#include "common/platform_types.h"
#include "common/utils/ds/byte_array.h"
#include "common/utils/eq_check.h"
#include "fgmm_lib.h"

#define REGISTRATION_REJECT_MIN_LEN 1

/** @brief Encode Registration Reject (8.2.7 of 3GPP TS 24.501) */
int encode_fgs_registration_reject(byte_array_t *buffer, const fgs_registration_reject_msg_t *msg)
{
  if (buffer->len < REGISTRATION_REJECT_MIN_LEN) {
    PRINT_ERROR("Failed to encode Registration Reject: missing Cause IE!\n");
    return -1;
  }

  return encode_fgs_nas_cause(buffer, &msg->cause);
}

/** @brief Decode Registration Reject (8.2.7 of 3GPP TS 24.501) — mandatory 5GMM cause only */
int decode_fgs_registration_reject(fgs_registration_reject_msg_t *msg, const byte_array_t *buffer)
{
  if (buffer->len < REGISTRATION_REJECT_MIN_LEN) {
    PRINT_ERROR("Nothing to decode: missing Cause IE!\n");
    return -1;
  }

  int decoded = decode_fgs_nas_cause(&msg->cause, buffer);
  if (decoded < 0) {
    return -1;
  }

  if (buffer->len > decoded) {
    PRINT_ERROR("Optional Registration Reject IEs present but not handled\n");
  }

  return decoded;
}

bool eq_registration_reject(const fgs_registration_reject_msg_t *a, const fgs_registration_reject_msg_t *b)
{
  _EQ_CHECK_INT(a->cause, b->cause);
  return true;
}

void free_fgs_registration_reject(fgs_registration_reject_msg_t *msg)
{
  UNUSED(msg);
}
