/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _NR_RLC_RX_MANAGER_H_
#define _NR_RLC_RX_MANAGER_H_

#include "nr_rlc_pdu.h"

typedef struct {
  /* 'rx_list' is managed as a circular buffer
   * starts at rx_list[start] where is stored 'sn_start'
   * for an RLC entity with an SN of n bits, its size is 2^(n-1)
   * corresponding the the window size
   */
  nr_rlc_pdu_t **rx_list;
  int size;
  int start;
  int sn_start;
} nr_rlc_rx_manager_t;

nr_rlc_rx_manager_t *nr_rlc_new_rx_manager(int size);
void nr_rlc_free_rx_manager(nr_rlc_rx_manager_t *m);
void nr_rlc_clear_rx_manager(nr_rlc_rx_manager_t *m);

nr_rlc_pdu_t *nr_rlc_rx_manager_get_pdu_from_sn(nr_rlc_rx_manager_t *m, int sn);
void nr_rlc_rx_manager_clear_pdu(nr_rlc_rx_manager_t *m, int sn);
void nr_rlc_rx_manager_advance(nr_rlc_rx_manager_t *m, int count);
void nr_rlc_rx_manager_set_start(nr_rlc_rx_manager_t *m, int sn_start);
void nr_rlc_rx_manager_add_pdu(nr_rlc_rx_manager_t *m, nr_rlc_pdu_t *pdu);

#endif /* _NR_RLC_RX_MANAGER_H_ */
