/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief rrc variables
 */


#ifndef __OPENAIR_RRC_VARS_H__
#define __OPENAIR_RRC_VARS_H__
#include "rrc_defs.h"
#include "LAYER2/RLC/rlc.h"
#include "COMMON/mac_rrc_primitives.h"
#include "LAYER2/MAC/mac.h"

UE_PF_PO_t UE_PF_PO[NFAPI_CC_MAX][MAX_MOBILES_PER_ENB];
pthread_mutex_t ue_pf_po_mutex;
UE_RRC_INST *UE_rrc_inst = NULL;
#include "LAYER2/MAC/mac_extern.h"
extern uint16_t ue_id_g;

long logicalChannelGroup0 = 0;
long logicalChannelSR_Mask_r9=0;

struct LTE_LogicalChannelConfig__ul_SpecificParameters LCSRB1 =  {1,
         LTE_LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity,
         0,
         &logicalChannelGroup0
};
struct LTE_LogicalChannelConfig__ul_SpecificParameters LCSRB2 =  {3,
         LTE_LogicalChannelConfig__ul_SpecificParameters__prioritisedBitRate_infinity,
         0,
         &logicalChannelGroup0
};

struct LTE_LogicalChannelConfig__ext1 logicalChannelSR_Mask_r9_ext1 = {
.logicalChannelSR_Mask_r9=
  &logicalChannelSR_Mask_r9
};

// These are the default SRB configurations from 36.331 (Chapter 9, p. 176-179 in v8.6)
LTE_LogicalChannelConfig_t  SRB1_logicalChannelConfig_defaultValue = {.ul_SpecificParameters=
                                                                      &LCSRB1,
                                                                      .ext1=
                                                                      &logicalChannelSR_Mask_r9_ext1
                                                                     };

const LTE_LogicalChannelConfig_t SRB2_logicalChannelConfig_defaultValue = {.ul_SpecificParameters = &LCSRB2,
                                                                           .ext1 = &logicalChannelSR_Mask_r9_ext1};

//CONSTANTS
LCHAN_DESC CCCH_LCHAN_DESC, DCCH_LCHAN_DESC;

// only used for RRC connection re-establishment procedure TS36.331 5.3.7
// [0]: current C-RNTI, [1]: prior C-RNTI
// insert one when eNB received RRCConnectionReestablishmentRequest message
// delete one when eNB received RRCConnectionReestablishmentComplete message
uint16_t reestablish_rnti_map[MAX_MOBILES_PER_ENB][2] = {{0}};

#endif
