/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef OAIORAN_H
#define OAIORAN_H

#include <stdint.h>
#include "xran_fh_o_du.h"

typedef struct {
  uint32_t tti;
  uint32_t sl;
  uint32_t f;
  uint8_t mu;
  // the first column can expand to XRAN_MAX_SECTOR_NR for the support of multiple CCs
  int32_t nRxPkt[1][XRAN_MAX_ANTENNA_NR][XRAN_NUM_OF_SYMBOL_PER_SLOT];
} oran_sync_info_t;

/** @brief xran callback for fronthaul RX, see xran_5g_fronthault_config(). */
void oai_xran_fh_rx_callback(void *pCallbackTag, xran_status_t status, uint8_t mu);
/** @brief xran callback for fronthaul PRACH RX, see xran_5g_prach_req(). */
void oai_xran_fh_rx_prach_callback(void *pCallbackTag, xran_status_t status, uint8_t mu);
/** @brief xran callback for time alignment, see xran_reg_physide_cb(). */
int oai_physide_dl_tti_call_back(void *param, uint8_t mu);

#endif /* OAIORAN_H */
