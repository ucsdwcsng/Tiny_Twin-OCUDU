/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef FGS_DEREGISTRATION_REQUEST_UE_ORIGINATING_H_
#define FGS_DEREGISTRATION_REQUEST_UE_ORIGINATING_H_

#include <stdint.h>
#include "FGSDeregistrationType.h"
#include "FGSMobileIdentity.h"
#include "MessageType.h"
#include "NasKeySetIdentifier.h"
#include "SecurityHeaderType.h"

/*
 * Message name: De-registration request (UE originating de-registration)
 * Description: This message is sent by the UE to the AMF. See TS24.501 table 8.2.12.1.1.
 * Significance: dual
 * Direction: UE to network
 */

typedef struct fgs_deregistration_request_ue_originating_msg_tag {
  /* Mandatory fields */
  FGSDeregistrationType deregistrationtype;
  NasKeySetIdentifier naskeysetidentifier;
  FGSMobileIdentity fgsmobileidentity;
} fgs_deregistration_request_ue_originating_msg;

int encode_fgs_deregistration_request_ue_originating(const fgs_deregistration_request_ue_originating_msg *registrationrequest,
                                                     uint8_t *buffer,
                                                     uint32_t len);

#endif /* ! defined(REGISTRATION_REQUEST_H_) */
