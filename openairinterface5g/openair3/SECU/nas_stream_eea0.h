/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef OAI_NAS_STREAM_EEA0_H
#define OAI_NAS_STREAM_EEA0_H

#include "secu_defs.h"

#include <stdlib.h>
#include <stdint.h>

void nas_stream_encrypt_eea0(nas_stream_cipher_t const *stream_cipher, uint8_t *out);

#endif
