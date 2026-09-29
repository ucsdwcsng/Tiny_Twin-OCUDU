/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "nrppa_gNB_ue_context.h"
#include "tree.h"
#include "T.h"
#include "common/utils/T/T.h"
#include "common/utils/utils.h"
#include "common/utils/LOG/log.h"

static RB_HEAD(nrppa_ue_map, nrppa_gNB_ue_context_s) nrppa_ue_head = RB_INITIALIZER(&nrppa_ue_head);

/* Generate the tree management functions prototypes */
RB_PROTOTYPE(nrppa_ue_map, nrppa_gNB_ue_context_s, entries, nrppa_gNB_compare_transaction_id);

static int nrppa_gNB_compare_transaction_id(struct nrppa_gNB_ue_context_s *p1, struct nrppa_gNB_ue_context_s *p2)
{
  if (p1->transaction_id > p2->transaction_id) {
    return 1;
  }

  if (p1->transaction_id < p2->transaction_id) {
    return -1;
  }

  return 0;
}

/* Generate the tree management functions */
RB_GENERATE(nrppa_ue_map, nrppa_gNB_ue_context_s, entries, nrppa_gNB_compare_transaction_id);

void nrppa_store_ue_context(const nrppa_gnb_ue_info_t *info, const uint16_t transaction_id)
{
  LOG_I(NRPPA, "Create NRPPa UE context (gNB UE ID %u) for transaction ID : %u\n", info->gNB_ue_ngap_id, transaction_id);
  nrppa_gNB_ue_context_t *ue_info_p = calloc_or_fail(1, sizeof(*ue_info_p));
  ue_info_p->transaction_id = transaction_id;
  ue_info_p->gNB_ue_ngap_id = info->gNB_ue_ngap_id;
  ue_info_p->amf_ue_ngap_id = info->amf_ue_ngap_id;
  if (info->routing_id.len > 0 && info->routing_id.buf != NULL) {
    ue_info_p->routing_id.len = info->routing_id.len;
    ue_info_p->routing_id.buf = calloc_or_fail(1, info->routing_id.len);
    memcpy(ue_info_p->routing_id.buf, info->routing_id.buf, info->routing_id.len);
  } else {
    ue_info_p->routing_id.len = 0;
    ue_info_p->routing_id.buf = NULL;
  }
  if (RB_INSERT(nrppa_ue_map, &nrppa_ue_head, ue_info_p))
    LOG_E(NRPPA, "Bug in UE uniq number allocation %u, we try to add a existing UE\n", ue_info_p->gNB_ue_ngap_id);
  return;
}

struct nrppa_gNB_ue_context_s *nrppa_get_ue_context(uint16_t transaction_id)
{
  nrppa_gNB_ue_context_t temp = {.transaction_id = transaction_id};
  return RB_FIND(nrppa_ue_map, &nrppa_ue_head, &temp);
}

struct nrppa_gNB_ue_context_s *nrppa_detach_ue_context(uint16_t transaction_id)
{
  struct nrppa_gNB_ue_context_s *tmp = nrppa_get_ue_context(transaction_id);
  if (tmp == NULL) {
    LOG_E(NRPPA, "Trying to free a NULL UE context, %u\n", transaction_id);
    return NULL;
  }
  RB_REMOVE(nrppa_ue_map, &nrppa_ue_head, tmp);
  return tmp;
}

void nrppa_free_ue_context(nrppa_gNB_ue_context_t *ue_info)
{
  if (!ue_info)
    return;

  if (ue_info->routing_id.buf) {
    free(ue_info->routing_id.buf);
  }
  free(ue_info);
}

struct nrppa_gNB_ue_context_s *nrppa_get_context_by_ue_id(uint32_t gNB_ue_ngap_id)
{
  struct nrppa_gNB_ue_context_s *ue_info = NULL;

  RB_FOREACH (ue_info, nrppa_ue_map, &nrppa_ue_head) {
    if (ue_info->gNB_ue_ngap_id == gNB_ue_ngap_id) {
      return ue_info;
    }
  }
  return NULL;
}
