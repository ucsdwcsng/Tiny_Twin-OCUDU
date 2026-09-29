/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief security mode complete procedures for gNB
 */

#include <stdint.h>
#include "FGCNasMessageContainer.h"
#include "FGSMobileIdentity.h"
#include "MessageType.h"
#include "SecurityHeaderType.h"

#ifndef FGS_NAS_SECURITY_MODE_COMPLETE_H_
#define FGS_NAS_SECURITY_MODE_COMPLETE_H_

/*
 * Message name: security mode complete
 * Description: This message is sent by the UE to the AMF in response to a SECURITY MODE COMMAND message. See table 8.2.26.1.1.
 * Significance: dual
 * Direction: UE to AMF
 */

typedef struct fgs_security_mode_complete_msg_tag {
  /* Mandatory fields */
  FGSMobileIdentity fgsmobileidentity;
  FGCNasMessageContainer fgsnasmessagecontainer;
} fgs_security_mode_complete_msg;

int encode_fgs_security_mode_complete(const fgs_security_mode_complete_msg *fgs_security_mode_comp, uint8_t *buffer, uint32_t len);

#endif /* ! defined(FGS_NAS_SECURITY_MODE_COMPLETE_H_) */
