/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */
#ifndef NR_POS_UE_CONTEXT_H_
#define NR_POS_UE_CONTEXT_H_

#include "common/platform_types.h"
#include "common/ran_context.h"

typedef struct positioning_activation_info_s {
  rnti_t rnti;
} positioning_activation_info_t;

void add_pos_act_ue_context(struct gNB_MAC_INST_s *mac, const rnti_t rnti);
positioning_activation_info_t *get_pos_act_ue_context(struct gNB_MAC_INST_s *mac, rnti_t rnti);
void rm_pos_act_ue_context(struct gNB_MAC_INST_s *mac, rnti_t rnti);

#endif /* NR_POS_UE_CONTEXT_H_ */
