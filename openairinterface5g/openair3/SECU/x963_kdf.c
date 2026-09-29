/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "x963_kdf.h"

#if OPENSSL_VERSION_NUMBER >= 0x30000000L

/* code for version 3.0 or greater */

#include <string.h>
#include "common/utils/assertions.h"
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/core_names.h>

void x963_kdf(const byte_array_t sharedsecret, const byte_array_t sharedinfo, size_t len, uint8_t out[len])
{
  DevAssert(sharedsecret.buf != NULL);
  DevAssert(sharedsecret.len > 0);
  DevAssert(out != NULL);
  DevAssert(len > 0);

  EVP_KDF *kdf = EVP_KDF_fetch(NULL, "X963KDF", NULL);
  DevAssert(kdf != NULL);

  EVP_KDF_CTX *kctx = EVP_KDF_CTX_new(kdf);
  DevAssert(kctx != NULL);

  EVP_KDF_free(kdf);

  OSSL_PARAM params[4], *p = params;

  *p++ = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST, SN_sha256, strlen(SN_sha256));
  *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY, sharedsecret.buf, sharedsecret.len);

  if (sharedinfo.buf != NULL && sharedinfo.len > 0) {
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_INFO, sharedinfo.buf, sharedinfo.len);
  }

  *p = OSSL_PARAM_construct_end();

  int rc = EVP_KDF_derive(kctx, out, len, params);
  DevAssert(rc == 1);

  EVP_KDF_CTX_free(kctx);
}

#endif