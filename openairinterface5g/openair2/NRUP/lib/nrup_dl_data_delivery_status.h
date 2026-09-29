/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NRUP_DL_DATA_DELIVERY_STATUS_H
#define NRUP_DL_DATA_DELIVERY_STATUS_H

#include <stdbool.h>
#include <stdint.h>

#include "common/utils/ds/byte_array_producer.h"
#include "nrup_common.h"

/* TS 38.425 Figure 5.5.2.2-1 format */
#define NRUP_DDDS_PDU_TYPE_SHIFT 4
#define NRUP_DDDS_HIGHEST_TRANSMITTED 3
#define NRUP_DDDS_HIGHEST_DELIVERED 2
#define NRUP_DDDS_FINAL_FRAME 1
#define NRUP_DDDS_LOST_PACKET_REPORT 0
#define NRUP_DDDS_DELIVERED_SN_RANGE 4
#define NRUP_DDDS_DATA_RATE 3
#define NRUP_DDDS_RETRANSMISSION 2
#define NRUP_DDDS_DELIVERED_RETRANSMISSION 1
#define NRUP_DDDS_CAUSE_REPORT 0
#define NRUP_DDDS_MIN_LEN 6

/** TS 38.425 §5.5.3.16-§5.5.3.17: lost NR-U SN range start/end */
typedef struct {
  uint32_t start;
  uint32_t end;
} nrup_sn_range_t;

/* TS 38.425 §5.5.3.15: {1..161} */
typedef struct {
  uint8_t n_ranges;
  nrup_sn_range_t ranges[NRUP_MAX_LOST_NRU_SN_RANGES];
} nrup_lost_nru_sn_ranges_t;

/* TS 38.425 §5.5.3.51: {1..255} */
typedef struct {
  uint8_t n_ranges;
  nrup_sn_range_t ranges[NRUP_MAX_DELIVERED_OOS_SN_RANGES];
} nrup_delivered_oos_sn_ranges_t;

/* TS 38.425 DL Data Delivery Status */
typedef struct {
  // Final Frame Indication
  bool final_frame_ind;
  // Desired buffer size (mandatory, octets 2–5)
  uint32_t desired_buffer_size;
  // Desired Data Rate (optional 4 octets)
  bool desired_data_rate_present;
  uint32_t desired_data_rate;
  // Lost NR-U SN ranges
  bool lost_nru_ranges_present;
  nrup_lost_nru_sn_ranges_t lost_nru_ranges;
  // Highest successfully delivered NR PDCP SN (optional 3 octets)
  bool highest_delivered_nr_pdcp_sn_present;
  uint32_t highest_delivered_nr_pdcp_sn;
  // Highest transmitted NR PDCP SN (optional 3 octets)
  bool highest_transmitted_nr_pdcp_sn_present;
  uint32_t highest_transmitted_nr_pdcp_sn;
  // Cause Value (optional 1 octet)
  bool cause_value_present;
  uint8_t cause_value;
  // Successfully delivered retransmitted NR PDCP SN (optional 3 octets)
  bool delivered_retransmitted_nr_pdcp_sn_present;
  uint32_t delivered_retransmitted_nr_pdcp_sn;
  // Retransmitted NR PDCP SN (optional 3 octets)
  bool retransmitted_nr_pdcp_sn_present;
  uint32_t retransmitted_nr_pdcp_sn;
  // Delivered out-of-sequence NR PDCP SN ranges
  bool delivered_oos_ranges_present;
  nrup_delivered_oos_sn_ranges_t delivered_oos_ranges;
} nrup_dl_data_delivery_status_t;

int encode_nrup_dl_data_delivery_status(byte_array_producer_t *b, const nrup_dl_data_delivery_status_t *msg);
bool decode_nrup_dl_data_delivery_status(const uint8_t *pdu, int pdu_len, nrup_dl_data_delivery_status_t *out);
bool eq_nrup_dl_data_delivery_status(const nrup_dl_data_delivery_status_t *a, const nrup_dl_data_delivery_status_t *b);

#endif /* NRUP_DL_DATA_DELIVERY_STATUS_H */
