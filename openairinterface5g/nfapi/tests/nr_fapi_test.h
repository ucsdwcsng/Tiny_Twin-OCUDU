/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef OPENAIRINTERFACE_NR_FAPI_TEST_H
#define OPENAIRINTERFACE_NR_FAPI_TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef _STANDALONE_TESTING_
#include "common/utils/LOG/log.h"
#endif
#include "nfapi_nr_interface.h"
#include "nfapi_nr_interface_scf.h"

#include "common/platform_types.h"

#define FILL_TLV(TlV, TaG, VaL) \
  do {                          \
    TlV.tl.tag = TaG;           \
    TlV.value = VaL;            \
  } while (0)

uint8_t rand8()
{
  return (rand() & 0xff);
}
uint16_t rand16()
{
  return rand8() << 8 | rand8();
}
uint32_t rand24()
{
  return rand16() << 8 | rand8();
}
uint32_t rand32()
{
  return rand24() << 8 | rand8();
}
uint64_t rand64()
{
  return (uint64_t)rand32() << 32 | rand32();
}

uint8_t rand8_range(uint8_t lower, uint8_t upper)
{
  return (rand() % (upper - lower + 1)) + lower;
}

uint16_t rand16_range(uint16_t lower, uint16_t upper)
{
  return (rand() % (upper - lower + 1)) + lower;
}

int16_t rands16_range(int16_t lower, int16_t upper)
{
  return (rand() % (upper - lower + 1)) + lower;
}

uint32_t rand32_range(uint32_t lower, uint32_t upper)
{
  return (rand() % (upper - lower + 1)) + lower;
}

static inline void fapi_test_init_seeded(time_t seed)
{
  srand(seed);
  printf("srand seed is %ld\n", seed);
  logInit();
  set_glog(OAILOG_DISABLE);
}

static inline void fapi_test_init()
{
  fapi_test_init_seeded(time(NULL));
}
#endif // OPENAIRINTERFACE_NR_FAPI_TEST_H
