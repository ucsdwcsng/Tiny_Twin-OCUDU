/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NRUP_DL_USER_DATA_H
#define NRUP_DL_USER_DATA_H

#include <stdbool.h>
#include <stdint.h>

#include "common/utils/ds/byte_array_producer.h"
#include "nrup_common.h"

/* TS 38.425 Figure 5.5.2.1-1 format */
#define NRUP_DL_USER_DATA_PDU_TYPE_SHIFT 4
#define NRUP_DL_USER_DATA_DL_DISCARD_BLOCKS 2
#define NRUP_DL_USER_DATA_DL_FLUSH 1
#define NRUP_DL_USER_DATA_REPORT_POLLING 0
#define NRUP_DL_USER_DATA_REQUEST_OUT_OF_SEQ_REPORT 4
#define NRUP_DL_USER_DATA_REPORT_DELIVERED 3
#define NRUP_DL_USER_DATA_USER_DATA_EXISTENCE 2
#define NRUP_DL_USER_DATA_ASSISTANCE_INFO_REPORT_POLLING 1
#define NRUP_DL_USER_DATA_RETRANSMISSION 0
#define NRUP_DL_USER_DATA_MIN_LEN 5
#define NRUP_DL_USER_DATA_DL_DISCARD_SN_LEN 3
#define NRUP_DL_USER_DATA_DISCARD_NUM_BLOCKS_LEN 1
#define NRUP_DL_USER_DATA_DISCARD_BLOCK_LEN 4
#define NRUP_DL_USER_DATA_REPORT_SN_LEN 3

/* TS 38.425 §5.5.3.11: {1..244} */
#define NRUP_MAX_DL_DISCARD_BLOCKS 244
#define NRUP_DISCARDED_BLOCK_SIZE_MIN 1
#define NRUP_DISCARDED_BLOCK_SIZE_MAX 255

typedef struct {
  uint32_t dl_discard_nr_pdcp_pdu_sn_start;
  uint8_t discarded_block_size;
} nrup_dl_discard_block_t;

typedef struct {
  bool report_polling;
  bool retransmission;
  bool assistance_info_report_polling;
  bool user_data_existence;
  bool request_out_of_seq_report;
  uint32_t nru_sequence_number;
  // DL discard NR PDCP PDU SN (signaled by DL Flush), 3 octets (TS 38.425 §5.5.3.7-§5.5.3.8)
  bool dl_flush;
  uint32_t dl_discard_nr_pdcp_pdu_sn;
  // DL Discard Blocks subfields (TS 38.425 §5.5.3.9-§5.5.3.12)
  bool dl_discard_blocks_present;
  uint8_t n_dl_discard_blocks;
  nrup_dl_discard_block_t dl_discard_blocks[NRUP_MAX_DL_DISCARD_BLOCKS];
  // DL report NR PDCP PDU SN (signaled by Report Delivered), 3 octets (TS 38.425 §5.5.3.41-§5.5.3.42)
  bool report_delivered;
  uint32_t nr_pdcp_pdu_sn;
} nrup_dl_user_data_t;

int encode_nrup_dl_user_data(byte_array_producer_t *b, const nrup_dl_user_data_t *msg);
bool decode_nrup_dl_user_data(const uint8_t *pdu, int pdu_len, nrup_dl_user_data_t *out);
bool eq_nrup_dl_user_data(const nrup_dl_user_data_t *a, const nrup_dl_user_data_t *b);

#endif /* NRUP_DL_USER_DATA_H */
