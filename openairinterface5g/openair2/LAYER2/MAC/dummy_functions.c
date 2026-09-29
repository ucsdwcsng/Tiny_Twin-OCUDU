/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "openair2/LAYER2/MAC/mac_proto.h"

// TODO temporary solution
//      to be removed once separation between NR and LTE is achieved

int sf_ahead = 4;
SCHEDULER_MODES global_scheduler_mode;

int rrc_mac_config_req_eNB(const module_id_t Mod_idP, const rrc_mac_config_req_eNB_t *param)
{
  return 0;
}

void mac_top_init_eNB(void)
{
}

uint32_t to_earfcn_UL(int eutra_bandP, long long int ul_CarrierFreq, uint32_t bw)
{
  return 0;
}

int32_t get_uldl_offset(int nr_bandP) {
  return 0;
}

IF_Module_t *IF_Module_init(int Mod_id)
{
  return NULL;
}

uint32_t from_earfcn(int eutra_bandP, uint32_t dl_earfcn)
{
  return 0;
}

uint32_t to_earfcn_DL(int eutra_bandP, long long int dl_CarrierFreq, uint32_t bw)
{
  return 0;
}
