/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef FGMM_AUTH_FAILURE_H
#define FGMM_AUTH_FAILURE_H

#include <stdint.h>
#include "fgmm_lib.h"
#include "common/utils/ds/byte_array.h"

typedef struct {
  // 5GMM cause (Mandatory)
  cause_id_t cause;
  // Authentication failure parameter (Optional)
  byte_array_t authentication_failure_param;
} fgmm_auth_failure_t;

int decode_fgmm_auth_failure(fgmm_auth_failure_t *msg, const byte_array_t *buffer);
int encode_fgmm_auth_failure(byte_array_t *buffer, const fgmm_auth_failure_t *msg);
bool eq_fgmm_auth_failure(const fgmm_auth_failure_t *a, const fgmm_auth_failure_t *b);
void free_fgmm_auth_failure(const fgmm_auth_failure_t *msg);

#endif /* FGMM_AUTH_FAILURE_H */
