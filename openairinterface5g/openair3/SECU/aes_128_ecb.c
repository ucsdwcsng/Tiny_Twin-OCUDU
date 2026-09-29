/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "aes_128_ecb.h"
#include "common/utils/assertions.h"
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/evp.h>

void aes_128_ecb(const aes_128_t* k_iv, byte_array_t msg, size_t len_out, uint8_t out[len_out])
{
  DevAssert(k_iv != NULL);
  DevAssert(k_iv->type == NONE_INITIALIZATION_VECTOR);

  // Create and initialise the context
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  DevAssert(ctx != NULL);

  // Initialise the encryption operation.
  int rc = EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, k_iv->key, NULL);
  DevAssert(rc == 1);

  int len_ev = 0;
  rc = EVP_EncryptUpdate(ctx, out, &len_ev, msg.buf, msg.len);
  AssertFatal(!(len_ev > len_out), "Buffer overflow");

  // Finalise the encryption. Normally ciphertext bytes may be written at
  // this stage, but this does not occur in GCM mode
  rc = EVP_EncryptFinal_ex(ctx, out + len_ev, &len_ev);
  DevAssert(rc == 1);

  // Get the tag
  // rc == EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
  EVP_CIPHER_CTX_free(ctx);
}
