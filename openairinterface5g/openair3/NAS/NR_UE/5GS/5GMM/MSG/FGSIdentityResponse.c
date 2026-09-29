/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief identity response procedures for gNB
 */

#include "FGSIdentityResponse.h"
#include <stdint.h>

int encode_fgmm_identity_response(uint8_t *buffer, const fgmm_identity_response_msg *fgs_identity_reps, uint32_t len)
{
  int encoded = 0;
  int encode_result = 0;

  if ((encode_result = encode_5gs_mobile_identity(&fgs_identity_reps->fgsmobileidentity, 0, buffer + encoded, len - encoded))
      < 0) // Return in case of error
    return encode_result;
  else
    encoded += encode_result;

  return encoded;
}
