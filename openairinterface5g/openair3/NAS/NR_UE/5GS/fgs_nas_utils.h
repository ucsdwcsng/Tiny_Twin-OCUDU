/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef FGS_NAS_UTILS_H
#define FGS_NAS_UTILS_H

#include <stdint.h>
#include <arpa/inet.h>
#include <string.h> // For memcpy
#include <stdio.h>
#include "utils.h" // text_info_t, TO_ENUM, TO_TEXT
#include "common/utils/eq_check.h"

#define GET_SHORT(input, size) ({           \
    uint16_t tmp16;                         \
    memcpy(&tmp16, (input), sizeof(tmp16)); \
    size += htons(tmp16);                   \
})

#endif // FGS_NAS_UTILS_H
