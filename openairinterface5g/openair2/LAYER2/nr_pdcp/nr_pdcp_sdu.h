/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _NR_PDCP_SDU_H_
#define _NR_PDCP_SDU_H_

#include <stdint.h>

#include "nr_pdcp_integrity_data.h"

typedef struct nr_pdcp_sdu_t {
  uint32_t                 count;
  char                     *buffer;
  int                      size;
  nr_pdcp_integrity_data_t msg_integrity;
  struct nr_pdcp_sdu_t     *next;
} nr_pdcp_sdu_t;

nr_pdcp_sdu_t *nr_pdcp_new_sdu(uint32_t count, char *buffer, int size,
                               const nr_pdcp_integrity_data_t *msg_integrity);
nr_pdcp_sdu_t *nr_pdcp_sdu_list_add(nr_pdcp_sdu_t *l, nr_pdcp_sdu_t *sdu);
int nr_pdcp_sdu_in_list(nr_pdcp_sdu_t *l, uint32_t count);
void nr_pdcp_free_sdu(nr_pdcp_sdu_t *sdu);

#endif /* _NR_PDCP_SDU_H_ */
