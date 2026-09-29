/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _NR_RLC_PDU_H_
#define _NR_RLC_PDU_H_

/**************************************************************************/
/* RX PDU management                                                      */
/**************************************************************************/

typedef struct nr_rlc_pdu_t {
  int sn;
  char *data;       /* contains only SDU bytes, no PDU header */
  int size;         /* size of SDU data, no PDU header bytes counted */
  int so;
  int is_first;
  int is_last;
  struct nr_rlc_pdu_t *next;
} nr_rlc_pdu_t;

nr_rlc_pdu_t *nr_rlc_new_pdu(int sn, int so, int is_first,
    int is_last, char *data, int size);

void nr_rlc_free_pdu(nr_rlc_pdu_t *pdu);

nr_rlc_pdu_t *nr_rlc_pdu_list_add(
    int (*sn_compare)(void *, int, int), void *sn_compare_data,
    nr_rlc_pdu_t *list, nr_rlc_pdu_t *pdu);

/**************************************************************************/
/* PDU decoder                                                            */
/**************************************************************************/

typedef struct {
  int error;
  int byte;           /* next byte to decode */
  int bit;            /* next bit in next byte to decode */
  char *buffer;
  int size;
} nr_rlc_pdu_decoder_t;

void nr_rlc_pdu_decoder_init(nr_rlc_pdu_decoder_t *decoder,
                             char *buffer, int size);

#define nr_rlc_pdu_decoder_in_error(d) ((d)->error == 1)

int nr_rlc_pdu_decoder_get_bits(nr_rlc_pdu_decoder_t *decoder, int count);

/**************************************************************************/
/* PDU encoder                                                            */
/**************************************************************************/

typedef struct {
  int byte;           /* next byte to encode */
  int bit;            /* next bit in next byte to encode */
  unsigned char *buffer;
  int size;
} nr_rlc_pdu_encoder_t;

void nr_rlc_pdu_encoder_init(nr_rlc_pdu_encoder_t *encoder,
                             char *buffer, int size);

void nr_rlc_pdu_encoder_put_bits(nr_rlc_pdu_encoder_t *encoder,
                                 int value, int count);

#endif /* _NR_RLC_PDU_H_ */
