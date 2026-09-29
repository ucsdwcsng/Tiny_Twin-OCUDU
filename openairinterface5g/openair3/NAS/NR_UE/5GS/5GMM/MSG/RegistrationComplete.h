/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdint.h>
#include "MessageType.h"
#include "SORTransparentContainer.h"
#include "SecurityHeaderType.h"

#ifndef REGISTRATION_COMPLETE_H_
#define REGISTRATION_COMPLETE_H_

typedef struct registration_complete_msg_tag {
  /* Optional fields */
  SORTransparentContainer *sortransparentcontainer;
} registration_complete_msg;

int decode_registration_complete(registration_complete_msg *registrationcomplete, const uint8_t *buffer, uint32_t len);

int encode_registration_complete(const registration_complete_msg *registrationcomplete, uint8_t *buffer, uint32_t len);

#endif /* ! defined(REGISTRATION_COMPLETE_H_) */
