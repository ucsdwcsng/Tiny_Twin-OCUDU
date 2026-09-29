/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef GNB_CONFIG_POSITIONING_H_
#define GNB_CONFIG_POSITIONING_H_

#include "common/utils/nr/nr_common.h"

positioning_config_t *RCconfig_nr_positioning(void);
void dump_positioning_config(const positioning_config_t *positioning_config);

#endif /* GNB_CONFIG_POSITIONING_H_ */
