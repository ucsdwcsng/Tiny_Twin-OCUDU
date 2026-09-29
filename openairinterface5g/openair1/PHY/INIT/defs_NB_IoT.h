/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */


#ifndef __INIT_DEFS_NB_IOT__H__
#define __INIT_DEFS_NB_IOT__H__

//#include "PHY/defs_NB_IoT.h"
#include "openair2/PHY_INTERFACE/IF_Module_NB_IoT.h"
#include "nfapi_interface.h"


/*brief Configure LTE_DL_FRAME_PARMS with components derived after initial synchronization (MIB-NB decoding + primary/secondary synch).*/
void phy_config_mib_eNB_NB_IoT(int      Mod_id,
                               int          eutra_band,
                               int          Nid_cell,
                               int          Ncp,
                               int      Ncp_UL,
                               int          p_eNB,
                               uint16_t   EARFCN,
                               uint16_t   prb_index, // NB_IoT_RB_ID,
                               uint16_t   operating_mode,
                               uint16_t   control_region_size,
                               uint16_t   eutra_NumCRS_ports);

/*NB_phy_config_sib1_eNB is not needed since NB-IoT use only FDD mode*/

/*brief Configure LTE_DL_FRAME_PARMS with components of SIB2-NB (at eNB).*/

//void NB_phy_config_sib2_eNB(module_id_t                            Mod_id,
//                         int                                    CC_id,
//                         RadioResourceConfigCommonSIB_NB_r13_t      *radioResourceConfigCommon
//                         );

void phy_config_sib2_eNB_NB_IoT(uint8_t Mod_id,
                                nfapi_nb_iot_config_t *config,
                                nfapi_rf_config_t *rf_config,
                                nfapi_uplink_reference_signal_config_t *ul_nrs_config,
                                extra_phyConfig_t *extra_phy_parms);

void phy_config_dedicated_eNB_NB_IoT(module_id_t Mod_id,
                                     rnti_t rnti,
                                     extra_phyConfig_t *extra_phy_parms);

// void phy_init_lte_top_NB_IoT(NB_IoT_DL_FRAME_PARMS *frame_parms);
void phy_init_nb_iot_eNB(PHY_VARS_eNB_NB_IoT *phyvar);
int l1_north_init_NB_IoT(void);

#endif

