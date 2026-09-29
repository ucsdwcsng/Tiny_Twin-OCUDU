/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef KEY_DERIVATION_FUNCTION_OSA_H
#define KEY_DERIVATION_FUNCTION_OSA_H

#include <stdint.h>
#include <stdlib.h>
#include "common/utils/ds/byte_array.h"

void kdf(const uint8_t key[32], byte_array_t data, size_t len, uint8_t out[len]);

#endif



