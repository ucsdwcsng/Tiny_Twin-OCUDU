/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "FGSDeregistrationRequestUEOriginating.h"
#include <stdint.h>

#define IEI_NULL 0x00

int encode_fgs_deregistration_request_ue_originating(const fgs_deregistration_request_ue_originating_msg *drr,
                                                     uint8_t *buffer,
                                                     uint32_t len)
{
  int encoded = 0;
  const FGSDeregistrationType *dt = &drr->deregistrationtype;
  *(buffer + encoded) = ((dt->switchoff & 0x1) << 7) | ((dt->reregistration_required & 0x1) << 6) | ((dt->access_type & 0x3) << 4);

  int encode_result;
  if ((encode_result = encode_nas_key_set_identifier(&drr->naskeysetidentifier, IEI_NULL)) < 0)
    return encode_result;

  encoded++;

  if ((encode_result = encode_5gs_mobile_identity(&drr->fgsmobileidentity, 0, buffer + encoded, len - encoded)) < 0)
    return encode_result;
  else
    encoded += encode_result;

  return encoded;
}
