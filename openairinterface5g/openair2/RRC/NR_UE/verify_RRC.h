/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*
 * \brief RRC functions prototypes for eNB and UE
 */

#ifndef _VERIFY_RRC_H_
#define _VERIFY_RRC_H_

#include "NR_MeasConfig.h"

bool check_csi_report_consistency(const NR_CSI_MeasConfig_t *meas);
bool check_cellgroup_config(const NR_CellGroupConfig_t *cgConfig);
#endif
