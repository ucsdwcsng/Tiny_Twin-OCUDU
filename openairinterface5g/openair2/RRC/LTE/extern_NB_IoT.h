/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief rrc external vars
 */

#ifndef __OPENAIR_RRC_EXTERN_NB_IOT_H__
#define __OPENAIR_RRC_EXTERN_NB_IOT_H__
#include "RRC/LTE/defs_NB_IoT.h"
#include "PHY_INTERFACE/IF_Module_NB_IoT.h"
#include "LAYER2/RLC/rlc.h"
#include "LTE_LogicalChannelConfig-NB-r13.h"
#include "LAYER2/MAC/defs_NB_IoT.h"

#include "common/ran_context.h"


//MP: NOTE:XXX some of the parameters defined in vars_nb_iot are called by the extern.h file so not replicated here

extern UE_RRC_INST_NB_IoT 					*UE_rrc_inst_NB_IoT;

extern eNB_RRC_INST_NB_IoT 					*eNB_rrc_inst_NB_IoT;
extern PHY_Config_NB_IoT_t 						*config_INFO;

extern LTE_LogicalChannelConfig_NB_r13_t 		SRB1bis_logicalChannelConfig_defaultValue_NB_IoT;
extern LTE_LogicalChannelConfig_NB_r13_t 		SRB1_logicalChannelConfig_defaultValue_NB_IoT;

extern uint16_t 							T300_NB_IoT[8];
extern uint16_t 							T301_NB_IoT[8];
extern uint16_t 							T310_NB_IoT[8];
extern uint16_t 							T311_NB_IoT[8];
extern uint16_t 							N310_NB_IoT[8];
extern uint16_t 							N311_NB_IoT[8];
extern uint8_t *get_NB_IoT_MIB(struct eNB_RRC_INST_NB_IoT_s *nb_iot_rrc);
#endif


