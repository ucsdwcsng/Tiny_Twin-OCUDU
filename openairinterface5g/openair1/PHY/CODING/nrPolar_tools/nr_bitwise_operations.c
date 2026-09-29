/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief
 */

#include "PHY/CODING/nrPolar_tools/nr_polar_defs.h"

void nr_bit2byte_uint32_8(uint32_t *in, uint16_t arraySize, uint8_t *out)
{
  const uint arrayInd = ceil(arraySize / 32.0);
  for (int i = 0; i < (arrayInd - 1); i++) {
    for (int j = 0; j < 32; j++) {
      out[j + (i * 32)] = (in[i] >> j) & 1;
    }
  }

  for (int j = 0; j < arraySize - ((arrayInd - 1) * 32); j++)
    out[j + ((arrayInd - 1) * 32)] = (in[(arrayInd - 1)] >> j) & 1;
}

void nr_byte2bit_uint8_32(uint8_t *in, uint16_t arraySize, uint32_t *out)
{
  const uint arrayInd = ceil(arraySize / 32.0);
  for (int i = 0; i < arrayInd; i++) {
    out[i] = 0;
    for (int j = 31; j > 0; j--) {
      out[i] |= in[(i * 32) + j];
      out[i] <<= 1;
    }
    out[i] |= in[(i * 32)];
  }
}
