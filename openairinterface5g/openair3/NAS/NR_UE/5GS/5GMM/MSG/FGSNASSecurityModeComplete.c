/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief security mode complete procedures for gNB
 */

#include "FGSNASSecurityModeComplete.h"
#include <stdint.h>
#include "FGCNasMessageContainer.h"
#include "FGSMobileIdentity.h"

int encode_fgs_security_mode_complete(const fgs_security_mode_complete_msg *fgs_security_mode_comp, uint8_t *buffer, uint32_t len)
{
  int encoded = 0;
  int encode_result = 0;

  if ((encode_result =
           encode_5gs_mobile_identity(&fgs_security_mode_comp->fgsmobileidentity, 0x77, buffer + encoded, len - encoded))
      < 0) { // Return in case of error
    return encode_result;
  } else {
    encoded += encode_result;
    if ((encode_result = encode_fgc_nas_message_container(&fgs_security_mode_comp->fgsnasmessagecontainer,
                                                          0x71,
                                                          buffer + encoded,
                                                          len - encoded))
        < 0) {
      return encode_result;
    } else {
      encoded += encode_result;
    }
  }

  return encoded;
}
