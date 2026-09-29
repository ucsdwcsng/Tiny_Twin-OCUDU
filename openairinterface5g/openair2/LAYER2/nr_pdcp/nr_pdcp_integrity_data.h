/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _NR_PDCP_INTEGRITY_DATA_H_
#define _NR_PDCP_INTEGRITY_DATA_H_

#include <stdint.h>

typedef struct {
  uint32_t count;
  uint8_t mac[4];
  uint8_t header_size;
  uint8_t header[3];
} nr_pdcp_integrity_data_t;

#endif /* _NR_PDCP_INTEGRITY_DATA_H_ */
