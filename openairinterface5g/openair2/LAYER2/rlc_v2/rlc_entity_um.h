/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _RLC_ENTITY_UM_H_
#define _RLC_ENTITY_UM_H_

#include "rlc_entity.h"
#include "rlc_pdu.h"
#include "rlc_sdu.h"

typedef struct {
  char sdu[SDU_MAX];     /* sdu is reassembled here */
  int  sdu_pos;          /* next byte to put in sdu */

  /* decoder of current PDU */
  rlc_pdu_decoder_t    dec;
  int sn;

  int sdu_head_missing;
} rlc_um_reassemble_t;

typedef struct {
  rlc_entity_t common;

  /* configuration */
  int t_reordering;
  int sn_field_length;

  int sn_modulus;        /* 1024 for sn_field_length == 10, 32 for 5 */
  int window_size;       /* 512 for sn_field_length == 10, 16 for 5 */

  /* runtime rx */
  int vr_ur;
  int vr_ux;
  int vr_uh;

  /* runtime tx */
  int vt_us;

  /* set to the latest know time by the user of the module. Unit: ms */
  uint64_t t_current;

  /* timers (stores the TTI of activation, 0 means not active) */
  uint64_t t_reordering_start;

  /* rx management */
  rlc_rx_pdu_segment_t *rx_list;
  int                  rx_size;
  int                  rx_maxsize;

  /* reassembly management */
  rlc_um_reassemble_t reassemble;

  /* tx management */
  rlc_sdu_t *tx_list;
  rlc_sdu_t *tx_end;
  int       tx_size;
  int       tx_maxsize;
} rlc_entity_um_t;

void rlc_entity_um_recv_sdu(rlc_entity_t *_entity, char *buffer, int size,
                            int sdu_id);
void rlc_entity_um_recv_pdu(rlc_entity_t *entity, char *buffer, int size);
rlc_entity_buffer_status_t rlc_entity_um_buffer_status(
    rlc_entity_t *entity, int maxsize);
int rlc_entity_um_generate_pdu(rlc_entity_t *_entity, char *buffer, int size);
void rlc_entity_um_set_time(rlc_entity_t *entity, uint64_t now);
void rlc_entity_um_discard_sdu(rlc_entity_t *entity, int sdu_id);
void rlc_entity_um_reestablishment(rlc_entity_t *entity);
void rlc_entity_um_delete(rlc_entity_t *entity);

#endif /* _RLC_ENTITY_UM_H_ */
