/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief 5GS Mobile Capability for registration request procedures
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "OctetString.h"

#ifndef FGMM_CAPABILITY_H_
#define FGMM_CAPABILITY_H_

typedef struct {
  uint8_t iei;
  uint8_t length;
  bool sgc;
  bool iphc_cp_cIoT;
  bool n3_data;
  bool cp_cIoT;
  bool restrict_ec;
  bool lpp;
  bool ho_attach;
  bool s1_mode;
  bool racs;
  bool nssaa;
  bool lcs;
  bool v2x_cnpc5;
  bool v2x_cepc5;
  bool v2x;
  bool up_cIoT;
  bool srvcc;
  bool ehc_CP_ciot;
  bool multiple_eUP;
  bool wusa;
  bool cag;
} FGMMCapability;

int encode_5gmm_capability(uint8_t *buffer, const FGMMCapability *fgmmcapability, uint32_t len);

#endif /* FGMM_CAPABILITY_H_ */
