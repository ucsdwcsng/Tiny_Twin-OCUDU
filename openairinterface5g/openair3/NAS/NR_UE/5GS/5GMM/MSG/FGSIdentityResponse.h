/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief identity response procedures for gNB
 */

#include <stdint.h>
#include "FGSMobileIdentity.h"
#include "MessageType.h"
#include "SecurityHeaderType.h"

#ifndef FGS_IDENTITY_RESPONSE_H_
#define FGS_IDENTITY_RESPONSE_H_

/*
 * Message name: Identity response
 * Description: This message is sent by the UE to the AMF to provide the requested identity. See table 8.2.22.1.
 * Significance: dual
 * Direction: UE to AMF
 */

typedef struct fgmm_identity_response_msg_s {
  /* Mandatory fields */
  FGSMobileIdentity fgsmobileidentity;
} fgmm_identity_response_msg;

int encode_fgmm_identity_response(uint8_t *buffer, const fgmm_identity_response_msg *fgs_identity_reps, uint32_t len);

#endif /* ! defined(FGS_IDENTITY_RESPONSE_H_) */
