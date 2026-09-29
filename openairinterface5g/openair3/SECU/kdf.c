/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "common/utils/assertions.h"
#include "openair3/SECU/sha_256_hmac.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#include "kdf.h"

void kdf(const uint8_t key[32], byte_array_t data, size_t len, uint8_t out[len])
{
  // Do we still need kdf function?
  sha_256_hmac(key, data, len, out);
}

