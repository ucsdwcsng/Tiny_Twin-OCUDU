/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef x963_KDF_OAI_H
#define x963_KDF_OAI_H

#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER >= 0x30000000L

/* code for version 3.0 or greater */

#include <stdint.h>
#include <stdlib.h>

#include "byte_array.h"

void x963_kdf(const byte_array_t sharedsecret, const byte_array_t sharedinfo, size_t len, uint8_t out[len]);

#endif

#endif
