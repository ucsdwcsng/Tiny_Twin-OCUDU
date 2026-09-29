/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "NasKeySetIdentifier.h"

#ifndef FGS_AUTHENTICATION_REQUEST_H_
#define FGS_AUTHENTICATION_REQUEST_H_

typedef struct {
  // NAS key set identifier (Mandatory)
  NasKeySetIdentifier ngKSI;
  // ABBA (Mandatory)
  int abba;
} fgs_authentication_request_t;

#endif /* ! defined(FGS_AUTHENTICATION_REQUEST_H_) */
