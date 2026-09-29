/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef OAI_NAS_STREAM_EIA2_H
#define OAI_NAS_STREAM_EIA2_H

#include "secu_defs.h"

void nas_stream_encrypt_eia2(nas_stream_cipher_t const* stream_cipher, uint8_t out[4]);

stream_security_context_t *stream_integrity_init_eia2(const uint8_t *integrity_key);
void stream_integrity_free_eia2(stream_security_context_t *integrity_context);

#endif
