/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdint.h>
#include <math.h>
#include "tools_defs.h"

#define LUT_SIN_RESOL 128
// look-up table for the sine (cosine) function
static uint16_t LUTSin[LUT_SIN_RESOL + 1];

void InitSinLUT(void)
{
  for (int i = 0; i < (LUT_SIN_RESOL + 1); i++) {
    LUTSin[i] = round(sin((M_PI * i) / (2 * LUT_SIN_RESOL)) * (1 << 14)); // Format: Q14
  }
}

// fast calculation of e^{i*2*pi*phase}
// ret.r == cosinus << 14
// ret.i == sinus << 14
c16_t get_sin_cos(double phase)
{
  int index;
  if (phase < 0) {
    index = (int)(-phase * 4 * LUT_SIN_RESOL + 0.5) % (4 * LUT_SIN_RESOL);
    index = 4 * LUT_SIN_RESOL - index;
  } else {
    index = (int)(phase * 4 * LUT_SIN_RESOL + 0.5) % (4 * LUT_SIN_RESOL);
  }

  c16_t ret;
  if (index < 2 * LUT_SIN_RESOL) { // check for 1st and 2nd Quadrant
    if (index < LUT_SIN_RESOL) { // 1st Quadrant
      ret.r = LUTSin[LUT_SIN_RESOL - index];
      ret.i = LUTSin[index];
    } else { // 2nd Quadrant
      ret.r = -LUTSin[index - LUT_SIN_RESOL];
      ret.i = LUTSin[2 * LUT_SIN_RESOL - index];
    }
  } else { // 3rd and 4th Quadrant
    if (index < 3 * LUT_SIN_RESOL) { // 3rd Quadrant
      ret.r = -LUTSin[3 * LUT_SIN_RESOL - index];
      ret.i = -LUTSin[index - 2 * LUT_SIN_RESOL];
    } else { // 4th Quadrant
      ret.r = LUTSin[index - 3 * LUT_SIN_RESOL];
      ret.i = -LUTSin[4 * LUT_SIN_RESOL - index];
    }
  }

  return ret;
}
