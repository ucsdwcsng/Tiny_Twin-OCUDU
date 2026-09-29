/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NRUP_COMMON_H
#define NRUP_COMMON_H

#include <stdbool.h>
#include <stdint.h>

#include "common/utils/ds/byte_array_producer.h"

/* TS 38.425 NR user plane protocol PDU types (octet 1, bits 8-5) */
#define NRUP_PDU_DL_USER_DATA 0
#define NRUP_PDU_DL_DATA_DELIVERY_STATUS 1

/* TS 38.425 §5.5.3 SN value ranges */
#define NRUP_NR_U_SN_MAX 0xffffffu
#define NRUP_NR_PDCP_SN_MAX 0x3ffffu

/* TS 38.425 §5.5.3.15 Number of lost NR-U SN ranges reported */
#define NRUP_MAX_LOST_NRU_SN_RANGES 161
/* TS 38.425 §5.5.3.51 Number of delivered out-of-sequence PDCP SN ranges */
#define NRUP_MAX_DELIVERED_OOS_SN_RANGES 255

typedef struct {
  const uint8_t *pdu;
  int pdu_len;
  int pos;
} nrup_pdu_reader_t;

/* TS 38.425 clause 5.5.3.24: pad so PDU length is n*4-2 */
static inline bool nrup_put_padding(byte_array_producer_t *b)
{
  while (b->pos % 4 != 2) {
    if (!byte_array_producer_put_byte(b, 0))
      return false;
  }
  return true;
}

#endif /* NRUP_COMMON_H */
