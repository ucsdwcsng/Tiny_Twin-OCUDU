/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef FGMM_AUTH_REJECT_H
#define FGMM_AUTH_REJECT_H

#include <stdint.h>
#include "fgmm_lib.h"
#include "common/utils/ds/byte_array.h"

typedef struct {
  // EAP Message (Optional)
  byte_array_t eap_msg;
} fgmm_auth_reject_msg_t;

int decode_fgmm_auth_reject(fgmm_auth_reject_msg_t *msg, const byte_array_t *buffer);
int encode_fgmm_auth_reject(byte_array_t *buffer, const fgmm_auth_reject_msg_t *msg);
bool eq_auth_reject(fgmm_auth_reject_msg_t *a, fgmm_auth_reject_msg_t *b);

#endif /* FGMM_AUTH_REJECT_H */
