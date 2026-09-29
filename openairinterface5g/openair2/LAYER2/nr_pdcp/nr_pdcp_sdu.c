/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "nr_pdcp_sdu.h"

#include <stdlib.h>
#include <string.h>

nr_pdcp_sdu_t *nr_pdcp_new_sdu(uint32_t count, char *buffer, int size,
                               const nr_pdcp_integrity_data_t *msg_integrity)
{
  nr_pdcp_sdu_t *ret = calloc(1, sizeof(nr_pdcp_sdu_t));
  if (ret == NULL)
    exit(1);
  ret->count = count;
  ret->buffer = malloc(size);
  if (ret->buffer == NULL)
    exit(1);
  memcpy(ret->buffer, buffer, size);
  ret->size = size;
  memcpy(&ret->msg_integrity, msg_integrity, sizeof(*msg_integrity));
  return ret;
}

nr_pdcp_sdu_t *nr_pdcp_sdu_list_add(nr_pdcp_sdu_t *l, nr_pdcp_sdu_t *sdu)
{
  nr_pdcp_sdu_t head;
  nr_pdcp_sdu_t *cur;
  nr_pdcp_sdu_t *prev;

  head.next = l;
  cur = l;
  prev = &head;

  /* order is by 'count' */
  while (cur != NULL) {
    /* check if 'sdu' is before 'cur' in the list */
    if (sdu->count < cur->count)
      break;
    prev = cur;
    cur = cur->next;
  }
  prev->next = sdu;
  sdu->next = cur;
  return head.next;
}

int nr_pdcp_sdu_in_list(nr_pdcp_sdu_t *l, uint32_t count)
{
  while (l != NULL) {
    if (l->count == count)
      return 1;
    l = l->next;
  }
  return 0;
}

void nr_pdcp_free_sdu(nr_pdcp_sdu_t *sdu)
{
  free(sdu->buffer);
  free(sdu);
}
