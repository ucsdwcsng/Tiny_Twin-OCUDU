/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "OctetString.h"

#ifndef AUTHENTICATION_RESPONSE_PARAMETER_H_
#define AUTHENTICATION_RESPONSE_PARAMETER_H_

#define AUTHENTICATION_RESPONSE_PARAMETER_MINIMUM_LENGTH 6
#define AUTHENTICATION_RESPONSE_PARAMETER_MAXIMUM_LENGTH 18

typedef struct AuthenticationResponseParameter_tag {
  OctetString res;
} AuthenticationResponseParameter;

int encode_authentication_response_parameter(const AuthenticationResponseParameter *authenticationresponseparameter,
                                             uint8_t iei,
                                             uint8_t *buffer,
                                             uint32_t len);

int decode_authentication_response_parameter(AuthenticationResponseParameter *authenticationresponseparameter, uint8_t iei, uint8_t *buffer, uint32_t len);

void dump_authentication_response_parameter_xml(AuthenticationResponseParameter *authenticationresponseparameter);

#endif /* AUTHENTICATION RESPONSE PARAMETER_H_ */

