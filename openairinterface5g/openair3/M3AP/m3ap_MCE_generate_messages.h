/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m2ap procedures for eNB
 */

#ifndef __M3AP_MCE_GENERATE_MESSAGES__H__
#define __M3AP_MCE_GENERATE_MESSAGES__H__

#include "m2ap_eNB_defs.h"
#include "m2ap_common.h"

int m2ap_eNB_generate_m2_setup_request(m2ap_eNB_instance_t *instance_p,
				       m2ap_eNB_data_t *m2ap_eNB_data_p);

int m2ap_MCE_generate_m2_setup_response(m2ap_eNB_instance_t *instance_p, m2ap_eNB_data_t *m2ap_eNB_data_p);

int m2ap_eNB_set_cause (M2AP_Cause_t * cause_p,
                        M2AP_Cause_PR cause_type,
                        long cause_value);

#endif /*  __M3AP_MCE_GENERATE_MESSAGES__H__ */
