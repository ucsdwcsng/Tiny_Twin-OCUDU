/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */
#include "nr_pos_ue_context.h"
#include "nr_mac_gNB.h"
#include "common/utils/ds/seq_arr.h"
#include "common/utils/LOG/log.h"
#include "common/utils/alg/find.h"

static bool eq_ue_rnti(const void *a, const void *b)
{
  const positioning_activation_info_t *ue_info = (const positioning_activation_info_t *)a;
  const rnti_t *target_rnti = (const rnti_t *)b;
  return ue_info->rnti == *target_rnti;
}

static elm_arr_t find_ue_by_rnti(gNB_MAC_INST *mac, rnti_t rnti)
{
  elm_arr_t elm = {0};
  elm = find_if(&mac->pos_act_ue_arr, &rnti, eq_ue_rnti);
  return elm;
}

void add_pos_act_ue_context(gNB_MAC_INST *mac, rnti_t rnti)
{
  elm_arr_t elm = find_ue_by_rnti(mac, rnti);
  if (elm.found) {
    LOG_W(NR_MAC, "Positioning context for RNTI 0x%04x already exists\n", rnti);
    return;
  }

  LOG_I(NR_MAC, "Create positioning UE context for RNTI : 0x%04x\n", rnti);
  positioning_activation_info_t new_ue_info = {
      .rnti = rnti,
  };
  seq_arr_push_back(&mac->pos_act_ue_arr, &new_ue_info, sizeof(positioning_activation_info_t));
}

positioning_activation_info_t *get_pos_act_ue_context(gNB_MAC_INST *mac, rnti_t rnti)
{
  elm_arr_t elm = find_ue_by_rnti(mac, rnti);
  if (elm.found)
    return (positioning_activation_info_t *)elm.it;
  return NULL;
}

void rm_pos_act_ue_context(gNB_MAC_INST *mac, rnti_t rnti)
{
  elm_arr_t elm = find_ue_by_rnti(mac, rnti);
  if (elm.found) {
    seq_arr_erase_deep(&mac->pos_act_ue_arr, elm.it, NULL);
  } else {
    LOG_E(NR_MAC, "Trying to detach a missing UE context, RNTI: 0x%04x\n", rnti);
  }
}
