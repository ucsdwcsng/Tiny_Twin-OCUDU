/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _NR_PARMS_H_
#define _NR_PARMS_H_
#include "fapi_nr_ue_interface.h"
#include "openair1/PHY/defs_nr_UE.h"

#ifdef __cplusplus
extern "C" {
#endif

void nr_init_frame_parms(nfapi_nr_config_request_scf_t *config, NR_DL_FRAME_PARMS *frame_parms);
int nr_init_frame_parms_ue(NR_DL_FRAME_PARMS *frame_parms, fapi_nr_config_request_t *config, uint16_t nr_band);
void nr_init_frame_parms_ue_sa(NR_DL_FRAME_PARMS *frame_parms, const nrUE_cell_params_t *cell);
int nr_init_frame_parms_ue_sl(NR_DL_FRAME_PARMS *fp,
                              sl_nr_phy_config_request_t *config,
                              int threequarter_fs,
                              uint32_t ofdm_offset_divisor);
void nr_dump_frame_parms(NR_DL_FRAME_PARMS *frame_parms);

#ifdef __cplusplus
}
#endif

#endif
