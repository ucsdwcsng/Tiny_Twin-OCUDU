/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m3ap procedures for MCE
 */

#ifndef M3AP_MCE_GENERATE_MESSAGES_H_
#define M3AP_MCE_GENERATE_MESSAGES_H_

#include "m2ap_MCE_defs.h"
#include "m2ap_common.h"

int m2ap_MCE_generate_m2_setup_request(m2ap_MCE_instance_t *instance_p,
				       m2ap_MCE_data_t *m2ap_MCE_data_p);

int m2ap_MCE_generate_m2_setup_response(m2ap_MCE_instance_t *instance_p, m2ap_MCE_data_t *m2ap_MCE_data_p);

int m2ap_MCE_generate_m2_setup_failure(instance_t instance,
                                       sctp_assoc_t assoc_id,
                                       M2AP_Cause_PR cause_type,
                                       long cause_value,
                                       long time_to_wait);

int m2ap_MCE_set_cause (M2AP_Cause_t * cause_p,
                        M2AP_Cause_PR cause_type,
                        long cause_value);

//int m2ap_MCE_generate_m2_handover_request (m2ap_MCE_instance_t *instance_p, m2ap_MCE_data_t *m2ap_MCE_data_p,
//                                           m2ap_handover_req_t *m2ap_handover_req, int ue_id);
//
//int m2ap_MCE_generate_m2_handover_request_ack (m2ap_MCE_instance_t *instance_p, m2ap_MCE_data_t *m2ap_MCE_data_p,
//                                               m2ap_handover_req_ack_t *m2ap_handover_req_ack);
//
//int m2ap_MCE_generate_m2_ue_context_release (m2ap_MCE_instance_t *instance_p, m2ap_MCE_data_t *m2ap_MCE_data_p,
//                                             m2ap_ue_context_release_t *m2ap_ue_context_release);
//
//int m2ap_MCE_generate_m2_handover_cancel (m2ap_MCE_instance_t *instance_p, m2ap_MCE_data_t *m2ap_MCE_data_p,
//                                          int m2_ue_id,
//                                          m2ap_handover_cancel_cause_t cause);

#endif /*  M2AP_MCE_GENERATE_MESSAGES_H_ */
