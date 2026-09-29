/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef CURVE_25519_OAI_H
#define CURVE_25519_OAI_H

#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER >= 0x30000000L

/* code for version 3.0 or greater */

#include <stdint.h>
#include <stddef.h>

void x25519_generate_keypair(uint8_t priv[32], uint8_t pub[32]);

void x25519_shared_secret(const uint8_t priv[32], const uint8_t peer_pub[32], uint8_t secret[32]);

#endif

#endif