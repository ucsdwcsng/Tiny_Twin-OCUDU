/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _RRC_GNB_ASN1_H_
#define _RRC_GNB_ASN1_H_

#include <stdbool.h>
#include "seq_arr.h"
#include "NR_DRB-ToAddMod.h"
#include "NR_RadioBearerConfig.h"
#include "openair2/LAYER2/nr_pdcp/nr_pdcp_configuration.h"

NR_PDCP_Config_t *nr_rrc_build_pdcp_config_ie(const bool integrity, const bool ciphering, const nr_pdcp_configuration_t *pdcp);

#endif
