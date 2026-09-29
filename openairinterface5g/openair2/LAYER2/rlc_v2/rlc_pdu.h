/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _RLC_PDU_H_
#define _RLC_PDU_H_

/**************************************************************************/
/* RX PDU segment and segment list                                        */
/**************************************************************************/

typedef struct rlc_rx_pdu_segment_t {
  int sn;
  int so;
  int size;
  int is_last;
  char *data;
  int data_offset;
  struct rlc_rx_pdu_segment_t *next;
} rlc_rx_pdu_segment_t;

rlc_rx_pdu_segment_t *rlc_rx_new_pdu_segment(int sn, int so, int size,
    int is_last, char *data, int data_offset);

void rlc_rx_free_pdu_segment(rlc_rx_pdu_segment_t *pdu_segment);

rlc_rx_pdu_segment_t *rlc_rx_pdu_segment_list_add(
    int (*sn_compare)(void *, int, int), void *sn_compare_data,
    rlc_rx_pdu_segment_t *list, rlc_rx_pdu_segment_t *pdu_segment);

/**************************************************************************/
/* TX PDU management                                                      */
/**************************************************************************/

typedef struct rlc_tx_pdu_segment_t {
  int       sn;
  void      *start_sdu;        /* real type is rlc_sdu_t * */
  int       sdu_start_byte;    /* starting byte in 'start_sdu' */
  int       so;                /* starting byte of the segment in full PDU */
  int       data_size;         /* number of data bytes (exclude header) */
  int       is_segment;
  int       is_last;
  int       retx_count;
  struct rlc_tx_pdu_segment_t *next;
} rlc_tx_pdu_segment_t;

rlc_tx_pdu_segment_t *rlc_tx_new_pdu(void);
void rlc_tx_free_pdu(rlc_tx_pdu_segment_t *pdu);
rlc_tx_pdu_segment_t *rlc_tx_pdu_list_append(rlc_tx_pdu_segment_t *list,
    rlc_tx_pdu_segment_t *pdu);
rlc_tx_pdu_segment_t *rlc_tx_pdu_list_add(
    int (*sn_compare)(void *, int, int), void *sn_compare_data,
    rlc_tx_pdu_segment_t *list, rlc_tx_pdu_segment_t *pdu_segment);

/**************************************************************************/
/* PDU decoder                                                            */
/**************************************************************************/

typedef struct {
  int error;
  int byte;           /* next byte to decode */
  int bit;            /* next bit in next byte to decode */
  char *buffer;
  int size;
} rlc_pdu_decoder_t;

void rlc_pdu_decoder_init(rlc_pdu_decoder_t *decoder, char *buffer, int size);

#define rlc_pdu_decoder_in_error(d) ((d)->error == 1)

int rlc_pdu_decoder_get_bits(rlc_pdu_decoder_t *decoder, int count);

void rlc_pdu_decoder_align(rlc_pdu_decoder_t *decoder);

/**************************************************************************/
/* PDU encoder                                                            */
/**************************************************************************/

typedef struct {
  int byte;           /* next byte to encode */
  int bit;            /* next bit in next byte to encode */
  char *buffer;
  int size;
} rlc_pdu_encoder_t;

void rlc_pdu_encoder_init(rlc_pdu_encoder_t *encoder, char *buffer, int size);

void rlc_pdu_encoder_put_bits(rlc_pdu_encoder_t *encoder, int value, int count);

void rlc_pdu_encoder_align(rlc_pdu_encoder_t *encoder);

#endif /* _RLC_PDU_H_ */
