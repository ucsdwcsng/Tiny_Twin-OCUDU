/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "RegistrationComplete.h"
#include <stdint.h>
#include "SORTransparentContainer.h"

int decode_registration_complete(registration_complete_msg *registration_complete, const uint8_t *buffer, uint32_t len)
{
  uint32_t decoded = 0;
  int decoded_result = 0;

  /* Decoding mandatory fields */
  if ((decoded_result =
           decode_sor_transparent_container(registration_complete->sortransparentcontainer, 0, buffer + decoded, len - decoded))
      < 0)
    return decoded_result;
  else
    decoded += decoded_result;

  return decoded;
}

int encode_registration_complete(const registration_complete_msg *registration_complete, uint8_t *buffer, uint32_t len)
{
  int encoded = 0;
  int encode_result = 0;

  if (!registration_complete->sortransparentcontainer)
    return encoded;

  if ((encode_result =
           encode_sor_transparent_container(registration_complete->sortransparentcontainer, 0, buffer + encoded, len - encoded))
      < 0) // Return in case of error
    return encode_result;
  else
    encoded += encode_result;

  return encoded;
}
