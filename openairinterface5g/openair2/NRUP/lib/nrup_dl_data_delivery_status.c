/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <string.h>

#include "common/utils/eq_check.h"
#include "LOG/log.h"
#include "openair3/UTILS/conversions.h"
#include "nrup_dl_data_delivery_status.h"
#include "nrup_common.h"

#define NRUP_DDDS_DATA_RATE_LEN 4
#define NRUP_DDDS_RANGE_COUNT_LEN 1
#define NRUP_DDDS_SN_RANGE_LEN 6
#define NRUP_DDDS_PDCP_SN_LEN 3
#define NRUP_DDDS_CAUSE_LEN 1

/* TS 38.425 Figure 5.5.2.2-1 octets 1-2 (LSB first) */
typedef struct {
  uint8_t lost_packet_report: 1;
  uint8_t final_frame_ind: 1;
  uint8_t highest_delivered_nr_pdcp_sn_ind: 1;
  uint8_t highest_transmitted_nr_pdcp_sn_ind: 1;
  uint8_t pdu_type: 4;
  uint8_t cause_report: 1;
  uint8_t delivered_retransmitted_nr_pdcp_sn_ind: 1;
  uint8_t retransmitted_nr_pdcp_sn_ind: 1;
  uint8_t data_rate_ind: 1;
  uint8_t delivered_nr_pdcp_sn_range_ind: 1;
  uint8_t spare: 3;
} __attribute__((packed)) nrup_ddds_hdr_t;
_Static_assert(sizeof(nrup_ddds_hdr_t) == 2, "DDDS flag octets");

static bool encode_sn_ranges(byte_array_producer_t *b, uint8_t max_n_ranges, uint8_t n_ranges, const nrup_sn_range_t *ranges)
{
  if (n_ranges > max_n_ranges)
    return false;

  if (!byte_array_producer_put_byte(b, n_ranges))
    return false;

  for (uint8_t i = 0; i < n_ranges; i++) {
    const nrup_sn_range_t *range = &ranges[i];
    if (!byte_array_producer_put_u24_be(b, range->start) || !byte_array_producer_put_u24_be(b, range->end))
      return false;
  }

  return true;
}

static bool decode_sn_ranges(nrup_pdu_reader_t *r, uint8_t max_n_ranges, uint8_t *n_ranges_out, nrup_sn_range_t *ranges)
{
  if (r->pos + NRUP_DDDS_RANGE_COUNT_LEN > r->pdu_len) {
    LOG_E(NR_UP,
          "Failed to decode NR-UP DL DATA DELIVERY STATUS: truncated SN range count (need %d, have %d)\n",
          r->pos + NRUP_DDDS_RANGE_COUNT_LEN,
          r->pdu_len);
    return false;
  }

  const uint8_t n_ranges = r->pdu[r->pos];
  if (n_ranges > max_n_ranges) {
    LOG_E(NR_UP, "Failed to decode NR-UP DL DATA DELIVERY STATUS: n_ranges %u > max %u\n", n_ranges, max_n_ranges);
    return false;
  }

  const int body_len = n_ranges * NRUP_DDDS_SN_RANGE_LEN;
  if (r->pos + NRUP_DDDS_RANGE_COUNT_LEN + body_len > r->pdu_len) {
    LOG_E(NR_UP,
          "Failed to decode NR-UP DL DATA DELIVERY STATUS: truncated SN ranges (n_ranges %u, need %d, have %d)\n",
          n_ranges,
          r->pos + NRUP_DDDS_RANGE_COUNT_LEN + body_len,
          r->pdu_len);
    return false;
  }

  *n_ranges_out = n_ranges;
  r->pos += NRUP_DDDS_RANGE_COUNT_LEN;

  for (uint8_t i = 0; i < n_ranges; i++) {
    ranges[i].start = ntoh_int24_buf(&r->pdu[r->pos]);
    ranges[i].end = ntoh_int24_buf(&r->pdu[r->pos + 3]);
    r->pos += NRUP_DDDS_SN_RANGE_LEN;
  }

  return true;
}

static bool encode_lost_nru_sn_ranges(byte_array_producer_t *b, const nrup_lost_nru_sn_ranges_t *ranges)
{
  return encode_sn_ranges(b, NRUP_MAX_LOST_NRU_SN_RANGES, ranges->n_ranges, ranges->ranges);
}

static bool encode_delivered_oos_sn_ranges(byte_array_producer_t *b, const nrup_delivered_oos_sn_ranges_t *ranges)
{
  return encode_sn_ranges(b, NRUP_MAX_DELIVERED_OOS_SN_RANGES, ranges->n_ranges, ranges->ranges);
}

static bool decode_lost_nru_sn_ranges(nrup_pdu_reader_t *r, nrup_lost_nru_sn_ranges_t *out)
{
  return decode_sn_ranges(r, NRUP_MAX_LOST_NRU_SN_RANGES, &out->n_ranges, out->ranges);
}

static bool decode_delivered_oos_sn_ranges(nrup_pdu_reader_t *r, nrup_delivered_oos_sn_ranges_t *out)
{
  return decode_sn_ranges(r, NRUP_MAX_DELIVERED_OOS_SN_RANGES, &out->n_ranges, out->ranges);
}

static bool eq_lost_nru_sn_ranges(const nrup_lost_nru_sn_ranges_t *a, const nrup_lost_nru_sn_ranges_t *b)
{
  _EQ_CHECK_INT(a->n_ranges, b->n_ranges);
  for (uint8_t i = 0; i < a->n_ranges; i++) {
    _EQ_CHECK_UINT32(a->ranges[i].start, b->ranges[i].start);
    _EQ_CHECK_UINT32(a->ranges[i].end, b->ranges[i].end);
  }

  return true;
}

static bool eq_delivered_oos_sn_ranges(const nrup_delivered_oos_sn_ranges_t *a, const nrup_delivered_oos_sn_ranges_t *b)
{
  _EQ_CHECK_INT(a->n_ranges, b->n_ranges);
  for (uint8_t i = 0; i < a->n_ranges; i++) {
    _EQ_CHECK_UINT32(a->ranges[i].start, b->ranges[i].start);
    _EQ_CHECK_UINT32(a->ranges[i].end, b->ranges[i].end);
  }

  return true;
}

/** @brief Encodes a DL Data Delivery Status message (TS 38.425 Figure 5.5.2.2-1)
 *
 * @param b The byte array producer to write to
 * @param msg The DL Data Delivery Status message to encode
 * @return 0 on error, 1 on success
 */
int encode_nrup_dl_data_delivery_status(byte_array_producer_t *b, const nrup_dl_data_delivery_status_t *msg)
{
  nrup_ddds_hdr_t hdr = {0};
  hdr.pdu_type = NRUP_PDU_DL_DATA_DELIVERY_STATUS;
  hdr.highest_transmitted_nr_pdcp_sn_ind = msg->highest_transmitted_nr_pdcp_sn_present;
  hdr.highest_delivered_nr_pdcp_sn_ind = msg->highest_delivered_nr_pdcp_sn_present;
  hdr.final_frame_ind = msg->final_frame_ind;
  hdr.lost_packet_report = msg->lost_nru_ranges_present;
  hdr.delivered_nr_pdcp_sn_range_ind = msg->delivered_oos_ranges_present;
  hdr.data_rate_ind = msg->desired_data_rate_present;
  hdr.retransmitted_nr_pdcp_sn_ind = msg->retransmitted_nr_pdcp_sn_present;
  hdr.delivered_retransmitted_nr_pdcp_sn_ind = msg->delivered_retransmitted_nr_pdcp_sn_present;
  hdr.cause_report = msg->cause_value_present;

  uint8_t raw[sizeof(hdr)];
  memcpy(raw, &hdr, sizeof(hdr));
  if (!byte_array_producer_put_byte(b, raw[0]) || !byte_array_producer_put_byte(b, raw[1]))
    return 0;

  if (!byte_array_producer_put_u32_be(b, msg->desired_buffer_size))
    return 0;

  /* TS 38.425 Figure 5.5.2.2-1: optional IE bodies in spec order after desired_buffer_size */
  if (hdr.data_rate_ind && !byte_array_producer_put_u32_be(b, msg->desired_data_rate))
    return 0;

  if (hdr.lost_packet_report && !encode_lost_nru_sn_ranges(b, &msg->lost_nru_ranges))
    return 0;

  if (hdr.highest_delivered_nr_pdcp_sn_ind && !byte_array_producer_put_u24_be(b, msg->highest_delivered_nr_pdcp_sn))
    return 0;

  if (hdr.highest_transmitted_nr_pdcp_sn_ind && !byte_array_producer_put_u24_be(b, msg->highest_transmitted_nr_pdcp_sn))
    return 0;

  if (hdr.cause_report && !byte_array_producer_put_byte(b, msg->cause_value))
    return 0;

  if (hdr.delivered_retransmitted_nr_pdcp_sn_ind && !byte_array_producer_put_u24_be(b, msg->delivered_retransmitted_nr_pdcp_sn))
    return 0;

  if (hdr.retransmitted_nr_pdcp_sn_ind && !byte_array_producer_put_u24_be(b, msg->retransmitted_nr_pdcp_sn))
    return 0;

  if (hdr.delivered_nr_pdcp_sn_range_ind && !encode_delivered_oos_sn_ranges(b, &msg->delivered_oos_ranges))
    return 0;

  if (!nrup_put_padding(b))
    return 0;

  return 1;
}

bool decode_nrup_dl_data_delivery_status(const uint8_t *pdu, int pdu_len, nrup_dl_data_delivery_status_t *out)
{
  DevAssert(pdu);
  DevAssert(out);
  if (pdu_len < NRUP_DDDS_MIN_LEN) {
    LOG_E(NR_UP, "Failed to decode NR-UP DL DATA DELIVERY STATUS: pdu_len %d < min length %d\n", pdu_len, NRUP_DDDS_MIN_LEN);
    return false;
  }

  memset(out, 0, sizeof(*out));

  nrup_ddds_hdr_t hdr;
  memcpy(&hdr, pdu, sizeof(hdr));

  if (hdr.pdu_type != NRUP_PDU_DL_DATA_DELIVERY_STATUS) {
    LOG_E(NR_UP, "Failed to decode NR-UP DL DATA DELIVERY STATUS: invalid PDU type %u\n", hdr.pdu_type);
    return false;
  }

  out->final_frame_ind = hdr.final_frame_ind;
  out->desired_buffer_size = ntoh_int32_buf(&pdu[2]);

  nrup_pdu_reader_t r = {
      .pdu = pdu,
      .pdu_len = pdu_len,
      .pos = NRUP_DDDS_MIN_LEN,
  };

  /* TS 38.425 Figure 5.5.2.2-1: optional IE bodies in spec order after desired_buffer_size */
  if (hdr.data_rate_ind) {
    if (r.pos + NRUP_DDDS_DATA_RATE_LEN > r.pdu_len) {
      LOG_E(NR_UP,
            "Failed to decode NR-UP DL DATA DELIVERY STATUS: truncated desired data rate (need %d, have %d)\n",
            r.pos + NRUP_DDDS_DATA_RATE_LEN,
            r.pdu_len);
      return false;
    }
    out->desired_data_rate_present = true;
    out->desired_data_rate = ntoh_int32_buf(&r.pdu[r.pos]);
    r.pos += NRUP_DDDS_DATA_RATE_LEN;
  }

  if (hdr.lost_packet_report) {
    out->lost_nru_ranges_present = true;
    if (!decode_lost_nru_sn_ranges(&r, &out->lost_nru_ranges))
      return false;
  }

  if (hdr.highest_delivered_nr_pdcp_sn_ind) {
    if (r.pos + NRUP_DDDS_PDCP_SN_LEN > r.pdu_len) {
      LOG_E(NR_UP,
            "Failed to decode NR-UP DL DATA DELIVERY STATUS: truncated highest delivered NR PDCP SN (need %d, have %d)\n",
            r.pos + NRUP_DDDS_PDCP_SN_LEN,
            r.pdu_len);
      return false;
    }
    out->highest_delivered_nr_pdcp_sn_present = true;
    out->highest_delivered_nr_pdcp_sn = ntoh_int24_buf(&r.pdu[r.pos]);
    r.pos += NRUP_DDDS_PDCP_SN_LEN;
  }

  if (hdr.highest_transmitted_nr_pdcp_sn_ind) {
    if (r.pos + NRUP_DDDS_PDCP_SN_LEN > r.pdu_len) {
      LOG_E(NR_UP,
            "Failed to decode NR-UP DL DATA DELIVERY STATUS: truncated highest transmitted NR PDCP SN (need %d, have %d)\n",
            r.pos + NRUP_DDDS_PDCP_SN_LEN,
            r.pdu_len);
      return false;
    }
    out->highest_transmitted_nr_pdcp_sn_present = true;
    out->highest_transmitted_nr_pdcp_sn = ntoh_int24_buf(&r.pdu[r.pos]);
    r.pos += NRUP_DDDS_PDCP_SN_LEN;
  }

  if (hdr.cause_report) {
    if (r.pos + NRUP_DDDS_CAUSE_LEN > r.pdu_len) {
      LOG_E(NR_UP,
            "Failed to decode NR-UP DL DATA DELIVERY STATUS: truncated cause value (need %d, have %d)\n",
            r.pos + NRUP_DDDS_CAUSE_LEN,
            r.pdu_len);
      return false;
    }
    out->cause_value_present = true;
    out->cause_value = r.pdu[r.pos];
    r.pos += NRUP_DDDS_CAUSE_LEN;
  }

  if (hdr.delivered_retransmitted_nr_pdcp_sn_ind) {
    if (r.pos + NRUP_DDDS_PDCP_SN_LEN > r.pdu_len) {
      LOG_E(NR_UP,
            "Failed to decode NR-UP DL DATA DELIVERY STATUS: truncated delivered retransmitted NR PDCP SN (need %d, have %d)\n",
            r.pos + NRUP_DDDS_PDCP_SN_LEN,
            r.pdu_len);
      return false;
    }
    out->delivered_retransmitted_nr_pdcp_sn_present = true;
    out->delivered_retransmitted_nr_pdcp_sn = ntoh_int24_buf(&r.pdu[r.pos]);
    r.pos += NRUP_DDDS_PDCP_SN_LEN;
  }

  if (hdr.retransmitted_nr_pdcp_sn_ind) {
    if (r.pos + NRUP_DDDS_PDCP_SN_LEN > r.pdu_len) {
      LOG_E(NR_UP,
            "Failed to decode NR-UP DL DATA DELIVERY STATUS: truncated retransmitted NR PDCP SN (need %d, have %d)\n",
            r.pos + NRUP_DDDS_PDCP_SN_LEN,
            r.pdu_len);
      return false;
    }
    out->retransmitted_nr_pdcp_sn_present = true;
    out->retransmitted_nr_pdcp_sn = ntoh_int24_buf(&r.pdu[r.pos]);
    r.pos += NRUP_DDDS_PDCP_SN_LEN;
  }

  if (hdr.delivered_nr_pdcp_sn_range_ind) {
    out->delivered_oos_ranges_present = true;
    if (!decode_delivered_oos_sn_ranges(&r, &out->delivered_oos_ranges))
      return false;
  }

  if (r.pos > r.pdu_len) {
    LOG_E(NR_UP, "Failed to decode NR-UP DL DATA DELIVERY STATUS: pdu_len %d < consumed %d\n", r.pdu_len, r.pos);
    return false;
  }

  /* TS 38.425 clause 5.5.1: ignore remaining bytes (Future Extension and/or padding) */
  return true;
}

bool eq_nrup_dl_data_delivery_status(const nrup_dl_data_delivery_status_t *a, const nrup_dl_data_delivery_status_t *b)
{
  if (a == NULL || b == NULL)
    return a == b;

  _EQ_CHECK_INT(a->final_frame_ind, b->final_frame_ind);
  _EQ_CHECK_UINT32(a->desired_buffer_size, b->desired_buffer_size);

  _EQ_CHECK_INT(a->desired_data_rate_present, b->desired_data_rate_present);
  if (a->desired_data_rate_present)
    _EQ_CHECK_UINT32(a->desired_data_rate, b->desired_data_rate);

  _EQ_CHECK_INT(a->lost_nru_ranges_present, b->lost_nru_ranges_present);
  if (a->lost_nru_ranges_present && !eq_lost_nru_sn_ranges(&a->lost_nru_ranges, &b->lost_nru_ranges))
    return false;

  _EQ_CHECK_INT(a->highest_delivered_nr_pdcp_sn_present, b->highest_delivered_nr_pdcp_sn_present);
  if (a->highest_delivered_nr_pdcp_sn_present)
    _EQ_CHECK_UINT32(a->highest_delivered_nr_pdcp_sn, b->highest_delivered_nr_pdcp_sn);

  _EQ_CHECK_INT(a->highest_transmitted_nr_pdcp_sn_present, b->highest_transmitted_nr_pdcp_sn_present);
  if (a->highest_transmitted_nr_pdcp_sn_present)
    _EQ_CHECK_UINT32(a->highest_transmitted_nr_pdcp_sn, b->highest_transmitted_nr_pdcp_sn);

  _EQ_CHECK_INT(a->cause_value_present, b->cause_value_present);
  if (a->cause_value_present)
    _EQ_CHECK_INT(a->cause_value, b->cause_value);

  _EQ_CHECK_INT(a->delivered_retransmitted_nr_pdcp_sn_present, b->delivered_retransmitted_nr_pdcp_sn_present);
  if (a->delivered_retransmitted_nr_pdcp_sn_present)
    _EQ_CHECK_UINT32(a->delivered_retransmitted_nr_pdcp_sn, b->delivered_retransmitted_nr_pdcp_sn);

  _EQ_CHECK_INT(a->retransmitted_nr_pdcp_sn_present, b->retransmitted_nr_pdcp_sn_present);
  if (a->retransmitted_nr_pdcp_sn_present)
    _EQ_CHECK_UINT32(a->retransmitted_nr_pdcp_sn, b->retransmitted_nr_pdcp_sn);

  _EQ_CHECK_INT(a->delivered_oos_ranges_present, b->delivered_oos_ranges_present);
  if (a->delivered_oos_ranges_present && !eq_delivered_oos_sn_ranges(&a->delivered_oos_ranges, &b->delivered_oos_ranges))
    return false;

  return true;
}
