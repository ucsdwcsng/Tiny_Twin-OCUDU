/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef GTPU_EXTENSIONS_H
#define GTPU_EXTENSIONS_H

#include <stdint.h>
#include <stdbool.h>

#include "nrup_dl_user_data.h"
#include "nrup_dl_data_delivery_status.h"

typedef enum {
  GTPU_EXT_NONE,
  /* 38.415 */
  GTPU_EXT_UL_PDU_SESSION_INFORMATION,
  /* 38.425 */
  GTPU_EXT_DL_DATA_DELIVERY_STATUS,
  GTPU_EXT_DL_USER_DATA,
} gtpu_extension_header_type_t;

/* 38.415 */
typedef struct {
  /* not all fields are present, to be refined if needed */
  bool qmp;
  bool dl_delay_ind;
  bool ul_delay_ind;
  bool snp;
  bool n3n9_delay_ind;
  bool new_ie_flag;
  int qfi;
} ul_pdu_session_information_t;

typedef struct {
  gtpu_extension_header_type_t type;
  union {
    ul_pdu_session_information_t ul_pdu_session_information;
    nrup_dl_data_delivery_status_t dl_data_delivery_status;
    nrup_dl_user_data_t dl_user_data;
  };
} gtpu_extension_header_t;

int serialize_gtpu_extension_type(gtpu_extension_header_type_t type);
int serialize_extension(gtpu_extension_header_t *ext, gtpu_extension_header_type_t next, uint8_t *out_buf, int out_len);

#endif /* GTPU_EXTENSIONS_H */
