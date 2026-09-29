/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef OAI_NAS_STREAM_EEA1_H
#define OAI_NAS_STREAM_EEA1_H

#include "secu_defs.h"

void nas_stream_encrypt_eea1(nas_stream_cipher_t const *stream_cipher, uint8_t *out);

stream_security_context_t *stream_ciphering_init_eea1(const uint8_t *ciphering_key);
void stream_ciphering_free_eea1(stream_security_context_t *ciphering_context);

#endif
