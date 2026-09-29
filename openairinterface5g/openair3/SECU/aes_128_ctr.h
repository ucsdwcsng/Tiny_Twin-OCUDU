/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef AES_128_CTR_OAI_H
#define AES_128_CTR_OAI_H

#include "aes_128.h"
#include "common/utils/ds/byte_array.h"
#include <endian.h>
#include <stdint.h>
#include <stdlib.h>

void aes_128_ctr(const aes_128_t* k_iv, byte_array_t msg, size_t len_out, uint8_t out[len_out]);

#endif
