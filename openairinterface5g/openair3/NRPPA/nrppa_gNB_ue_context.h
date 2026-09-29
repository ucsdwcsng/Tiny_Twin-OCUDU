/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdint.h>
#include "tree.h"
#include "ds/byte_array.h"
#include "nrppa_common.h"

#ifndef NRPPA_GNB_UE_CONTEXT_H_
#define NRPPA_GNB_UE_CONTEXT_H_

typedef struct nrppa_gNB_ue_context_s {
  /* Tree related data */
  RB_ENTRY(nrppa_gNB_ue_context_s) entries;
  uint16_t transaction_id;
  uint32_t gNB_ue_ngap_id;
  uint64_t amf_ue_ngap_id;
  byte_array_t routing_id;
} nrppa_gNB_ue_context_t;

void nrppa_store_ue_context(const nrppa_gnb_ue_info_t *info, const uint16_t transaction_id);
nrppa_gNB_ue_context_t *nrppa_get_ue_context(uint16_t transaction_id);
nrppa_gNB_ue_context_t *nrppa_detach_ue_context(uint16_t transaction_id);
void nrppa_free_ue_context(nrppa_gNB_ue_context_t *ue_info);
struct nrppa_gNB_ue_context_s *nrppa_get_context_by_ue_id(uint32_t gNB_ue_ngap_id);

#endif /* NRPPA_GNB_UE_CONTEXT_H_ */
