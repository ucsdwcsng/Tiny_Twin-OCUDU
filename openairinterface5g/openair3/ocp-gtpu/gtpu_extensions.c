/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "gtpu_extensions.h"
#include "common/utils/assertions.h"
#include "common/utils/ds/byte_array_producer.h"
#include "LOG/log.h"

/* 29.281 Figure 5.2.1-3 */
#define NR_RAN_CONTAINER        0x84
#define PDU_SESSION_CONTAINER   0x85

/* from an extension type, returns its "extension header type"
 * as defined in 29.281 Figure 5.2.1-3
 */
int serialize_gtpu_extension_type(gtpu_extension_header_type_t type)
{
  switch (type) {
    case GTPU_EXT_NONE:
      /* 0, no more extension */
      return 0;
    case GTPU_EXT_UL_PDU_SESSION_INFORMATION:
      return PDU_SESSION_CONTAINER;
    case GTPU_EXT_DL_DATA_DELIVERY_STATUS:
    case GTPU_EXT_DL_USER_DATA:
      return NR_RAN_CONTAINER;
    default:
      AssertFatal(0, "unknown GTPU extension type %d\n", type);
  }
}

/* returns 0 on error, 1 on success */
static int serialize_ul_pdu_session_information(byte_array_producer_t *b, ul_pdu_session_information_t *ext)
{
  /* see 38.415 5.5.2.2 */
  AssertFatal(!ext->qmp && !ext->dl_delay_ind && !ext->ul_delay_ind
              && !ext->snp && !ext->n3n9_delay_ind && !ext->new_ie_flag,
              "todo\n");

  uint8_t b1 = (1 << 4) | (ext->qmp << 3) | (ext->dl_delay_ind << 2) | (ext->ul_delay_ind << 1) | ext->snp;
  uint8_t b2 = (ext->n3n9_delay_ind << 7) | (ext->new_ie_flag << 6) | ext->qfi;
  return byte_array_producer_put_byte(b, b1) && byte_array_producer_put_byte(b, b2);
}

/** @brief TS 29.281 clause 5.2.1 Figure 5.2.1-1: serialize one GTP-U extension header
 *         (length, content, padding, next type)
 * @return -1 on error, number of serialized bytes on success */
int serialize_extension(gtpu_extension_header_t *ext, gtpu_extension_header_type_t next, uint8_t *out_buf, int out_len)
{
  byte_array_producer_t b;

  b = byte_array_producer_from_buffer(out_buf, out_len);
  /* octet 1: Extension Header Length in 4-octet units, filled once total size is known */
  if (!byte_array_producer_put_byte(&b, 0))
    goto error;

  switch (ext->type) {
    case GTPU_EXT_UL_PDU_SESSION_INFORMATION:
      if (!serialize_ul_pdu_session_information(&b, &ext->ul_pdu_session_information))
        goto error;
      break;
    case GTPU_EXT_DL_DATA_DELIVERY_STATUS:
      /* TS 38.425 Figure 5.5.2.2-1: NR-UP payload only */
      if (!encode_nrup_dl_data_delivery_status(&b, &ext->dl_data_delivery_status))
        goto error;
      break;
    case GTPU_EXT_DL_USER_DATA:
      /* TS 38.425 Figure 5.5.2.1-1: NR-UP payload only */
      if (!encode_nrup_dl_user_data(&b, &ext->dl_user_data))
        goto error;
      break;
    default:
      LOG_E(GTPU, "unknown extension type %d\n", ext->type);
      return -1;
  }

  /* TS 29.281 Figure 5.2.1-1: pad content to a 4-octet boundary before Next Extension Header Type */
  while ((b.pos & 3) != 3)
    if (!byte_array_producer_put_byte(&b, 0))
      goto error;

  /* TS 29.281 Figure 5.2.1-3: Next Extension Header Type (0 = no more extensions) */
  if (!byte_array_producer_put_byte(&b, serialize_gtpu_extension_type(next)))
    goto error;

  /* back-fill octet 1: total extension size in 4-octet units */
  DevAssert(b.pos / 4 <= 255);
  out_buf[0] = b.pos / 4;

  return b.pos;

error:
  LOG_E(GTPU, "error serializing extension\n");
  return -1;
}
