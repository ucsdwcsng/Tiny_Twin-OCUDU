/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _RLC_SDU_H_
#define _RLC_SDU_H_

typedef struct rlc_sdu_t {
  int upper_layer_id;
  char *data;
  int size;
  /* next_byte indicates the starting byte to use to construct a new PDU */
  int next_byte;
  int acked_bytes;
  struct rlc_sdu_t *next;
} rlc_sdu_t;

rlc_sdu_t *rlc_new_sdu(char *buffer, int size, int upper_layer_id);
void rlc_free_sdu(rlc_sdu_t *sdu);
void rlc_sdu_list_add(rlc_sdu_t **list, rlc_sdu_t **end, rlc_sdu_t *sdu);

#endif /* _RLC_SDU_H_ */
