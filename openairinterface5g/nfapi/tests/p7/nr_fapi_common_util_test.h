/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */
#ifndef OPENAIRINTERFACE_NR_FAPI_COMMON_UTIL_TEST_H
#define OPENAIRINTERFACE_NR_FAPI_COMMON_UTIL_TEST_H
#include "nfapi/tests/nr_fapi_test.h"

static uint8_t get_num_spatial_streams(void)
{
  const uint16_t max_logical_ant_ports = MAX_NUM_SPATIAL_STREAMS;
  return rand8_range(0, max_logical_ant_ports);
}

static void fill_spatial_streams(const uint8_t num_s, uint16_t s[MAX_NUM_SPATIAL_STREAMS])
{
  for (int n = 0; n < num_s; n++)
    s[n] = rand16_range(0, num_s - 1);
}

#endif
