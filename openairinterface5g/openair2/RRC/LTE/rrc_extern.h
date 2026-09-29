/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief rrc external vars
 */

#ifndef __OPENAIR_RRC_EXTERN_H__
#define __OPENAIR_RRC_EXTERN_H__
#include "rrc_defs.h"
#include "COMMON/mac_rrc_primitives.h"
#include "LAYER2/MAC/mac.h"
#include "LAYER2/RLC/rlc.h"
#include "openair2/RRC/LTE/rrc_defs.h"

extern UE_RRC_INST *UE_rrc_inst;

extern LTE_LogicalChannelConfig_t SRB1_logicalChannelConfig_defaultValue;
extern LTE_LogicalChannelConfig_t SRB2_logicalChannelConfig_defaultValue;

extern int NB_UE_INST;
extern void* bigphys_malloc(int);
extern LCHAN_DESC CCCH_LCHAN_DESC, DCCH_LCHAN_DESC;

extern UE_PF_PO_t UE_PF_PO[NFAPI_CC_MAX][MAX_MOBILES_PER_ENB];
extern pthread_mutex_t ue_pf_po_mutex;

extern uint16_t reestablish_rnti_map[MAX_MOBILES_PER_ENB][2];

#endif


