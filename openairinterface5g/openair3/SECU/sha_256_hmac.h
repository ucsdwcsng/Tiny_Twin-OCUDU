/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef SHA_256_HMAC_OAI_H
#define SHA_256_HMAC_OAI_H 

#include <stdint.h>
#include <stdlib.h>

#include "byte_array.h"

void sha_256_hmac(const uint8_t key[32], byte_array_t data, size_t len, uint8_t out[len]);

#endif

