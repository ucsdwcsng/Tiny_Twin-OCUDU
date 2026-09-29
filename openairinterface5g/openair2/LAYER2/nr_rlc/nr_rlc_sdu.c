/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "nr_rlc_sdu.h"

#include <stdlib.h>
#include <string.h>

#include "LOG/log.h"
#include "common/utils/assertions.h"

nr_rlc_sdu_segment_t *nr_rlc_new_sdu(
    char *buffer, int size,
    int upper_layer_id)
{
  /* allocate sdu header and data together */
  nr_rlc_sdu_t *sdu = malloc(sizeof(nr_rlc_sdu_t) + size);
  nr_rlc_sdu_segment_t *ret = calloc(1, sizeof(nr_rlc_sdu_segment_t));

  DevAssert(sdu != NULL);
  DevAssert(ret != NULL);

  /* only memset the header */
  memset(sdu, 0 , sizeof(*sdu));

  sdu->ref_count      = 1;
  sdu->sn             = -1;                 /* set later */
  sdu->upper_layer_id = upper_layer_id;
  sdu->data           = (char*)(sdu + 1);
  memcpy(sdu->data, buffer, size);
  sdu->size           = size;
  sdu->retx_count     = -1;

  ret->sdu      = sdu;
  ret->size     = size;
  ret->so       = 0;
  ret->is_first = 1;
  ret->is_last  = 1;

  return ret;
}

int nr_rlc_free_sdu_segment(nr_rlc_sdu_segment_t *sdu)
{
  int ret = 0;

  sdu->sdu->free_count++;
  if (sdu->sdu->free_count == sdu->sdu->ref_count) {
    free(sdu->sdu);
    ret = 1;
  }
  free(sdu);

  return ret;
}

void nr_rlc_sdu_segment_list_append(nr_rlc_sdu_segment_t **list,
                                    nr_rlc_sdu_segment_t **end,
                                    nr_rlc_sdu_segment_t *sdu)
{
  if (*list == NULL) {
    *list = sdu;
    *end = sdu;
    return;
  }

  (*end)->next = sdu;
  *end = sdu;
}

void nr_rlc_free_sdu_segment_list(nr_rlc_sdu_segment_t *l)
{
  nr_rlc_sdu_segment_t *cur;

  while (l != NULL) {
    cur = l;
    l = l->next;
    nr_rlc_free_sdu_segment(cur);
  }
}
