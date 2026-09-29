/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef __PHY_VARS_H__
#define __PHY_VARS_H__

#include "PHY/defs_UE.h"

#include "openair2/NR_PHY_INTERFACE/NR_IF_Module.h"
#include "openair2/PHY_INTERFACE/IF_Module.h"


PHY_VARS_UE ***PHY_vars_UE_g;
UL_RCC_IND_t UL_RCC_INFO;
int number_of_cards;
uint8_t max_turbo_iterations=4;

#endif /*__PHY_VARS_H__ */
