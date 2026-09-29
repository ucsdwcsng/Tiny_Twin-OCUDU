/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef FGS_DEREGISTRATION_TYPE_H_
#define FGS_DEREGISTRATION_TYPE_H_

#include <stdint.h>
#include "OctetString.h"


typedef struct FGSDeregistrationType_tag {
#define NORMAL_DEREGISTRATION 0
#define SWITCH_OFF 1
  uint8_t switchoff: 1;
#define REREGISTRATION_NOT_REQUIRED 0
#define REREGISTRATION_REQUIRED 1
  uint8_t reregistration_required: 1;
#define TGPP_ACCESS 1
#define NON_TGPP_ACCESS 2
#define TGPP_AND_NON_TGPP_ACCESS 3
  uint8_t access_type: 2;
} FGSDeregistrationType;

int encode_fgs_deregistration_type(FGSDeregistrationType *dt, uint8_t iei, uint8_t *buffer, uint32_t len);

#endif /* FGS_DEREGISTRATION_TYPE_H_ */
