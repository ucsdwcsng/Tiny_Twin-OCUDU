/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <string.h>

#include "common/utils/eq_check.h"
#include "LOG/log.h"
#include "openair3/UTILS/conversions.h"
#include "nrup_dl_user_data.h"
#include "nrup_common.h"

/* TS 38.425 Figure 5.5.2.1-1 octets 1-2 (LSB first) */
typedef struct {
  uint8_t report_polling: 1;
  uint8_t dl_flush: 1;
  uint8_t dl_discard_blocks: 1;
  uint8_t spare1: 1;
  uint8_t pdu_type: 4;
  uint8_t retransmission: 1;
  uint8_t assistance_info_report_polling: 1;
  uint8_t user_data_existence: 1;
  uint8_t report_delivered: 1;
  uint8_t request_out_of_seq_report: 1;
  uint8_t spare2: 3;
} __attribute__((packed)) nrup_dl_user_data_hdr_t;
_Static_assert(sizeof(nrup_dl_user_data_hdr_t) == 2, "DL USER DATA flag octets");

/** @brief Encode a NR-UP DL USER DATA PDU (TS 38.425 Figure 5.5.2.1-1)
 * @param b The byte array producer to encode to
 * @param msg The message to encode
 * @return 1 if the PDU was encoded successfully, 0 otherwise */
int encode_nrup_dl_user_data(byte_array_producer_t *b, const nrup_dl_user_data_t *msg)
{
  DevAssert(msg);
  DevAssert(b);

  nrup_dl_user_data_hdr_t hdr = {0};
  hdr.pdu_type = NRUP_PDU_DL_USER_DATA;
  hdr.dl_discard_blocks = msg->dl_discard_blocks_present;
  hdr.dl_flush = msg->dl_flush;
  hdr.report_polling = msg->report_polling;
  hdr.request_out_of_seq_report = msg->request_out_of_seq_report;
  hdr.report_delivered = msg->report_delivered;
  hdr.user_data_existence = msg->user_data_existence;
  hdr.assistance_info_report_polling = msg->assistance_info_report_polling;
  hdr.retransmission = msg->retransmission;

  uint8_t raw[sizeof(hdr)];
  memcpy(raw, &hdr, sizeof(hdr));
  if (!byte_array_producer_put_byte(b, raw[0]) || !byte_array_producer_put_byte(b, raw[1]))
    return 0;

  if (!byte_array_producer_put_u24_be(b, msg->nru_sequence_number))
    return 0;

  /* TS 38.425 Figure 5.5.2.1-1: optional IE bodies in spec order after NR-U SN */
  if (hdr.dl_flush && !byte_array_producer_put_u24_be(b, msg->dl_discard_nr_pdcp_pdu_sn))
    return 0;

  if (hdr.dl_discard_blocks) {
    if (msg->n_dl_discard_blocks > NRUP_MAX_DL_DISCARD_BLOCKS)
      return 0;
    if (!byte_array_producer_put_byte(b, msg->n_dl_discard_blocks))
      return 0;
    for (uint8_t i = 0; i < msg->n_dl_discard_blocks; i++) {
      const nrup_dl_discard_block_t *block = &msg->dl_discard_blocks[i];
      if (!byte_array_producer_put_u24_be(b, block->dl_discard_nr_pdcp_pdu_sn_start)
          || !byte_array_producer_put_byte(b, block->discarded_block_size))
        return 0;
    }
  }

  if (hdr.report_delivered && !byte_array_producer_put_u24_be(b, msg->nr_pdcp_pdu_sn))
    return 0;

  if (!nrup_put_padding(b))
    return 0;

  return 1;
}

/** @brief Decode a NR-UP DL USER DATA PDU (TS 38.425 Figure 5.5.2.1-1)
 * @param pdu The PDU to decode
 * @param pdu_len The length of the PDU
 * @param out The decoded message
 * @return true if the PDU was decoded successfully, false otherwise */
bool decode_nrup_dl_user_data(const uint8_t *pdu, int pdu_len, nrup_dl_user_data_t *out)
{
  DevAssert(pdu);
  DevAssert(out);
  if (pdu_len < NRUP_DL_USER_DATA_MIN_LEN) {
    LOG_E(NR_UP, "Failed to decode NR-UP DL USER DATA: pdu_len %d < min length %d\n", pdu_len, NRUP_DL_USER_DATA_MIN_LEN);
    return false;
  }

  memset(out, 0, sizeof(*out));

  nrup_dl_user_data_hdr_t hdr;
  memcpy(&hdr, pdu, sizeof(hdr));

  if (hdr.pdu_type != NRUP_PDU_DL_USER_DATA) {
    LOG_E(NR_UP, "Failed to decode NR-UP DL USER DATA: invalid PDU type %u\n", hdr.pdu_type);
    return false;
  }

  out->report_polling = hdr.report_polling;
  out->request_out_of_seq_report = hdr.request_out_of_seq_report;
  out->user_data_existence = hdr.user_data_existence;
  out->assistance_info_report_polling = hdr.assistance_info_report_polling;
  out->retransmission = hdr.retransmission;

  out->nru_sequence_number = ntoh_int24_buf(&pdu[2]);

  nrup_pdu_reader_t r = {
      .pdu = pdu,
      .pdu_len = pdu_len,
      .pos = NRUP_DL_USER_DATA_MIN_LEN,
  };

  /* TS 38.425 Figure 5.5.2.1-1: optional IE bodies in spec order after NR-U SN */

  if (hdr.dl_flush) {
    if (r.pos + NRUP_DL_USER_DATA_DL_DISCARD_SN_LEN > r.pdu_len) {
      LOG_E(NR_UP,
            "Failed to decode NR-UP DL USER DATA: truncated DL discard NR PDCP PDU SN (need %d, have %d)\n",
            r.pos + NRUP_DL_USER_DATA_DL_DISCARD_SN_LEN,
            r.pdu_len);
      return false;
    }
    out->dl_flush = true;
    out->dl_discard_nr_pdcp_pdu_sn = ntoh_int24_buf(&r.pdu[r.pos]);
    r.pos += NRUP_DL_USER_DATA_DL_DISCARD_SN_LEN;
  }

  if (hdr.dl_discard_blocks) {
    if (r.pos + NRUP_DL_USER_DATA_DISCARD_NUM_BLOCKS_LEN > r.pdu_len) {
      LOG_E(NR_UP,
            "Failed to decode NR-UP DL USER DATA: truncated DL discard Number of blocks (need %d, have %d)\n",
            r.pos + NRUP_DL_USER_DATA_DISCARD_NUM_BLOCKS_LEN,
            r.pdu_len);
      return false;
    }
    const uint8_t n_blocks = r.pdu[r.pos];
    if (n_blocks > NRUP_MAX_DL_DISCARD_BLOCKS) {
      LOG_E(NR_UP, "Failed to decode NR-UP DL USER DATA: n_blocks %u > max %u\n", n_blocks, NRUP_MAX_DL_DISCARD_BLOCKS);
      return false;
    }
    const int body_len = n_blocks * NRUP_DL_USER_DATA_DISCARD_BLOCK_LEN;
    if (r.pos + NRUP_DL_USER_DATA_DISCARD_NUM_BLOCKS_LEN + body_len > r.pdu_len) {
      LOG_E(NR_UP,
            "Failed to decode NR-UP DL USER DATA: truncated DL discard blocks (n_blocks %u, need %d, have %d)\n",
            n_blocks,
            r.pos + NRUP_DL_USER_DATA_DISCARD_NUM_BLOCKS_LEN + body_len,
            r.pdu_len);
      return false;
    }
    out->dl_discard_blocks_present = true;
    out->n_dl_discard_blocks = n_blocks;
    r.pos += NRUP_DL_USER_DATA_DISCARD_NUM_BLOCKS_LEN;
    for (uint8_t i = 0; i < n_blocks; i++) {
      out->dl_discard_blocks[i].dl_discard_nr_pdcp_pdu_sn_start = ntoh_int24_buf(&r.pdu[r.pos]);
      out->dl_discard_blocks[i].discarded_block_size = r.pdu[r.pos + 3];
      r.pos += NRUP_DL_USER_DATA_DISCARD_BLOCK_LEN;
    }
  }

  if (hdr.report_delivered) {
    if (r.pos + NRUP_DL_USER_DATA_REPORT_SN_LEN > r.pdu_len) {
      LOG_E(NR_UP,
            "Failed to decode NR-UP DL USER DATA: truncated DL report NR PDCP PDU SN (need %d, have %d)\n",
            r.pos + NRUP_DL_USER_DATA_REPORT_SN_LEN,
            r.pdu_len);
      return false;
    }
    out->report_delivered = true;
    out->nr_pdcp_pdu_sn = ntoh_int24_buf(&r.pdu[r.pos]);
    r.pos += NRUP_DL_USER_DATA_REPORT_SN_LEN;
  }

  if (r.pos > r.pdu_len) {
    LOG_E(NR_UP, "Failed to decode NR-UP DL USER DATA: pdu_len %d < consumed %d\n", r.pdu_len, r.pos);
    return false;
  }

  /* TS 38.425 clause 5.5.1: ignore remaining octets (Future Extension and/or padding) */
  return true;
}

bool eq_nrup_dl_user_data(const nrup_dl_user_data_t *a, const nrup_dl_user_data_t *b)
{
  if (a == NULL || b == NULL)
    return a == b;

  _EQ_CHECK_INT(a->dl_flush, b->dl_flush);
  if (a->dl_flush)
    _EQ_CHECK_UINT32(a->dl_discard_nr_pdcp_pdu_sn, b->dl_discard_nr_pdcp_pdu_sn);

  _EQ_CHECK_INT(a->dl_discard_blocks_present, b->dl_discard_blocks_present);
  if (a->dl_discard_blocks_present) {
    _EQ_CHECK_INT(a->n_dl_discard_blocks, b->n_dl_discard_blocks);
    for (uint8_t i = 0; i < a->n_dl_discard_blocks; i++) {
      _EQ_CHECK_UINT32(a->dl_discard_blocks[i].dl_discard_nr_pdcp_pdu_sn_start,
                       b->dl_discard_blocks[i].dl_discard_nr_pdcp_pdu_sn_start);
      _EQ_CHECK_INT(a->dl_discard_blocks[i].discarded_block_size, b->dl_discard_blocks[i].discarded_block_size);
    }
  }

  _EQ_CHECK_INT(a->report_polling, b->report_polling);
  _EQ_CHECK_INT(a->request_out_of_seq_report, b->request_out_of_seq_report);
  _EQ_CHECK_INT(a->user_data_existence, b->user_data_existence);
  _EQ_CHECK_INT(a->assistance_info_report_polling, b->assistance_info_report_polling);
  _EQ_CHECK_INT(a->retransmission, b->retransmission);
  _EQ_CHECK_UINT32(a->nru_sequence_number, b->nru_sequence_number);

  _EQ_CHECK_INT(a->report_delivered, b->report_delivered);
  if (a->report_delivered)
    _EQ_CHECK_UINT32(a->nr_pdcp_pdu_sn, b->nr_pdcp_pdu_sn);

  return true;
}
