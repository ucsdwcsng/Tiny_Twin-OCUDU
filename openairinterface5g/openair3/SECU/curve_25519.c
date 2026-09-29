/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "curve_25519.h"

#if OPENSSL_VERSION_NUMBER >= 0x30000000L

/* code for version 3.0 or greater */

#include "common/utils/assertions.h"

#include <openssl/evp.h>
#include <openssl/core_names.h>

void x25519_generate_keypair(uint8_t priv[32], uint8_t pub[32])
{
  DevAssert(priv != NULL);
  DevAssert(pub != NULL);

  EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, NULL);
  DevAssert(pctx != NULL);

  int rc = EVP_PKEY_keygen_init(pctx);
  DevAssert(rc == 1);

  EVP_PKEY *pkey = NULL;
  rc = EVP_PKEY_keygen(pctx, &pkey);
  DevAssert(rc == 1);

  EVP_PKEY_CTX_free(pctx);

  size_t len = 32;

  rc = EVP_PKEY_get_raw_private_key(pkey, priv, &len);
  DevAssert(rc == 1 && len == 32);

  rc = EVP_PKEY_get_raw_public_key(pkey, pub, &len);
  DevAssert(rc == 1 && len == 32);

  EVP_PKEY_free(pkey);
}

void x25519_shared_secret(const uint8_t priv[32], const uint8_t peer_pub[32], uint8_t secret[32])
{
  DevAssert(priv != NULL);
  DevAssert(peer_pub != NULL);
  DevAssert(secret != NULL);

  EVP_PKEY *pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, NULL, priv, 32);
  DevAssert(pkey != NULL);

  EVP_PKEY *peerkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, NULL, peer_pub, 32);
  DevAssert(peerkey != NULL);

  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
  DevAssert(ctx != NULL);

  int rc = EVP_PKEY_derive_init(ctx);
  DevAssert(rc == 1);

  rc = EVP_PKEY_derive_set_peer(ctx, peerkey);
  DevAssert(rc == 1);

  size_t len = 32;
  rc = EVP_PKEY_derive(ctx, secret, &len);
  DevAssert(rc == 1 && len == 32);

  EVP_PKEY_free(pkey);
  EVP_PKEY_free(peerkey);
  EVP_PKEY_CTX_free(ctx);
}

#endif