/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief rrc procedures for UE context
 */
#ifndef __RRC_ENB_UE_CONTEXT_H__
#define __RRC_ENB_UE_CONTEXT_H__

#include "collection/tree.h"
#include "common/platform_types.h"
#include "rrc_defs.h"

int rrc_eNB_compare_ue_rnti_id(
  struct rrc_eNB_ue_context_s* c1_pP,
  struct rrc_eNB_ue_context_s* c2_pP
);

RB_PROTOTYPE(rrc_ue_tree_s, rrc_eNB_ue_context_s, entries, rrc_eNB_compare_ue_rnti_id);

struct rrc_eNB_ue_context_s*
rrc_eNB_allocate_new_UE_context(
  eNB_RRC_INST* rrc_instance_pP
);

struct rrc_eNB_ue_context_s*
rrc_eNB_get_ue_context(
  eNB_RRC_INST* rrc_instance_pP,
  rnti_t rntiP
);

struct rrc_eNB_ue_context_s *
rrc_eNB_find_ue_context_from_gnb_rnti(
  eNB_RRC_INST *rrc_instance_pP,
  int gnb_rnti);

void rrc_eNB_remove_ue_context(
  const protocol_ctxt_t* const ctxt_pP,
  eNB_RRC_INST*                rrc_instance_pP,
  struct rrc_eNB_ue_context_s* ue_context_pP
);

#endif
