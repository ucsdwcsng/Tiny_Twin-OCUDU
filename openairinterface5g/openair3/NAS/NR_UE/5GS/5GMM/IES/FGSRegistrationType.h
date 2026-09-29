/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief 5GS Registration Type for registration request procedures
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "OctetString.h"

#ifndef FGS_REGISTRATION_TYPE_H_
#define FGS_REGISTRATION_TYPE_H_


typedef uint8_t FGSRegistrationType;

// clang-format off
/* 3GPP TS 24.501 Table 9.11.3.7.1: 5GS registration type IE */
#define INITIAL_REGISTRATION               0b001
#define MOBILITY_REGISTRATION_UPDATING     0b010
#define PERIODIC_REGISTRATION_UPDATING     0b011
#define EMERGENCY_REGISTRATION             0b100
#define REG_TYPE_RESERVED                  0b111
// clang-format on

int encode_5gs_registration_type(const FGSRegistrationType *fgsregistrationtype, bool follow_on_request);

int decode_5gs_registration_type(FGSRegistrationType *fgsregistrationtype, uint8_t iei, uint8_t value);

#endif /* FGS_REGISTRATION_TYPE_H_*/
