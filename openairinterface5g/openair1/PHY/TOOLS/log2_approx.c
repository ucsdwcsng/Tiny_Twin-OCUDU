/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <simde/x86/avx512/lzcnt.h>

unsigned char log2_approx(unsigned int x)
{
  const unsigned int round_val = 3037000499U; // 2^31.5
  if (!x)
    return 0;

  int l2 = simde_x_clz32(x);
  if (x > (round_val >> l2))
    l2 = 32 - l2;
  else
    l2 = 31 - l2;

  return l2;
}


unsigned char factor2(unsigned int x)
{
  unsigned char l2 = 0;
  int i;
  for (i = 0; i < 31; i++)
    if ((x & (1 << i)) != 0)
      break;

  l2 = i;
  return l2;
}

unsigned char log2_approx64(unsigned long long int x)
{
  const unsigned long long round_val = 13043817825332782212ULL; // 2^63.5
  if (!x)
    return 0;

  int l2 = simde_x_clz64(x);
  if (x > (round_val >> l2))
    l2 = 64 - l2;
  else
    l2 = 63 - l2;

  return l2;
}

