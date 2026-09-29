/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _SNOW3G_H_
#define _SNOW3G_H_

#include <stdint.h>

void snow3g_ciphering(uint32_t count,
                      int bearer,
                      int direction,
                      const uint8_t *key,
                      int length,
                      const uint8_t *in,
                      uint8_t *out);
void snow3g_integrity(uint32_t count,
                      int bearer,
                      int direction,
                      const uint8_t *key,
                      int length,
                      const uint8_t *in,
                      uint8_t *out);

#endif /* _SNOW3G_H_ */
