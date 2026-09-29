/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief ngap UE context management within gNB
 */ 

#include "ngap_gNB_ue_context.h"
#include <stdint.h>
#include <stdio.h>
#include "T.h"
#include "common/utils/T/T.h"
#include "ngap_common.h"
#include "tree.h"
#include "intertask_interface.h"
#define SCTP_LOOKUP_TIMER_TIMEOUT_SEC 30

/* Tree of UE ordered by gNB_ue_ngap_id's
 * NO INSTANCE, the 32 bits id is large enough to handle all UEs, regardless the cell, gNB, ...
 */
static RB_HEAD(ngap_ue_map, ngap_gNB_ue_context_s) ngap_ue_head = RB_INITIALIZER(&ngap_ue_head);

/* Generate the tree management functions prototypes */
RB_PROTOTYPE(ngap_ue_map, ngap_gNB_ue_context_s, entries, ngap_gNB_compare_gNB_ue_ngap_id);

static int ngap_gNB_compare_gNB_ue_ngap_id(struct ngap_gNB_ue_context_s *p1, struct ngap_gNB_ue_context_s *p2)
{
  if (p1->gNB_ue_ngap_id > p2->gNB_ue_ngap_id) {
    return 1;
  }

  if (p1->gNB_ue_ngap_id < p2->gNB_ue_ngap_id) {
    return -1;
  }

  return 0;
}

/* Generate the tree management functions */
RB_GENERATE(ngap_ue_map, ngap_gNB_ue_context_s, entries,
            ngap_gNB_compare_gNB_ue_ngap_id);

void ngap_store_ue_context(const ngap_gNB_ue_context_t *ue)
{
  LOG_I(NGAP, "Create UE context (ID %d) for AMF '%s' (assoc_id %d)\n", ue->gNB_ue_ngap_id, ue->amf_ref->amf_name, ue->amf_ref->assoc_id);
  ngap_gNB_ue_context_t *ue_desc_p = calloc_or_fail(1, sizeof(*ue_desc_p));
  *ue_desc_p = *ue;
  if (RB_INSERT(ngap_ue_map, &ngap_ue_head, ue_desc_p))
    LOG_E(NGAP, "Bug in UE uniq number allocation %u, we try to add a existing UE\n", ue_desc_p->gNB_ue_ngap_id);
  return;
}

struct ngap_gNB_ue_context_s *ngap_get_ue_context(uint32_t gNB_ue_ngap_id)
{
  ngap_gNB_ue_context_t temp = {.gNB_ue_ngap_id = gNB_ue_ngap_id};
  return RB_FIND(ngap_ue_map, &ngap_ue_head, &temp);
}

ngap_gNB_ue_context_t *ngap_get_ue_context_from_amf_ue_ngap_id(uint32_t amf_ue_ngap_id)
{
  ngap_gNB_ue_context_t temp = {.amf_ue_ngap_id = amf_ue_ngap_id};
  return RB_FIND(ngap_ue_map, &ngap_ue_head, &temp);
}

struct ngap_gNB_ue_context_s *ngap_detach_ue_context(uint32_t gNB_ue_ngap_id)
{
  struct ngap_gNB_ue_context_s *tmp = ngap_get_ue_context(gNB_ue_ngap_id);
  if (tmp == NULL) {
    NGAP_ERROR("Trying to free a NULL UE context, %u\n", gNB_ue_ngap_id);
    return NULL;
  }
  RB_REMOVE(ngap_ue_map, &ngap_ue_head, tmp);
  return tmp;
}

void ngap_release_ues_for_amf(ngap_gNB_amf_data_t *amf_desc_p)
{
  /* Release all UE contexts for this AMF */
  ngap_gNB_ue_context_t *ue = NULL;
  ngap_gNB_ue_context_t *next = NULL;
  RB_FOREACH_SAFE(ue, ngap_ue_map, &ngap_ue_head, next) {
    if (ue->amf_ref == amf_desc_p) {
      LOG_I(NGAP, "Releasing UE context: gNB UE NGAP ID: %u | AMF UE NGAP ID: %lu\n", ue->gNB_ue_ngap_id, ue->amf_ue_ngap_id);
      MessageDef *msg_p = itti_alloc_new_message(TASK_NGAP, 0, NGAP_UE_CONTEXT_RELEASE_COMMAND);
      NGAP_UE_CONTEXT_RELEASE_COMMAND(msg_p).gNB_ue_ngap_id = ue->gNB_ue_ngap_id;
      itti_send_msg_to_task(TASK_RRC_GNB, amf_desc_p->ngap_gNB_instance->instance, msg_p);
    }
  }
  /* Reset AMF connection and set reconnect timer */
  /* SCTP lookup timer */
  long tid;
  int rc = timer_setup(SCTP_LOOKUP_TIMER_TIMEOUT_SEC,
                       0,
                       TASK_NGAP,
                       amf_desc_p->ngap_gNB_instance->instance,
                       TIMER_PERIODIC,
                       amf_desc_p,
                       &tid);
  if (rc == 0) {
    amf_desc_p->t_reconnect = tid;
  }
}
