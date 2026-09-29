/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef FGS_IDENTITY_REQUEST_H
#define FGS_IDENTITY_REQUEST_H

#include <stdint.h>
#include <stdbool.h>
#include "common/utils/ds/byte_array.h"
#include "FGSMobileIdentity.h"

typedef struct {
  fgs_identity_type_t fgsmobileidentity;
} fgs_identity_request_msg_t;

int decode_fgs_identity_request(fgs_identity_request_msg_t *msg, const byte_array_t *buffer);

#endif /* FGS_IDENTITY_REQUEST_H */
