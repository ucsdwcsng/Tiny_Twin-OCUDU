/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m2ap handler procedures for eNB
 */

#ifndef M2AP_ENB_HANDLERS_H_
#define M2AP_ENB_HANDLERS_H_

#include "m2ap_eNB_defs.h"

//void m2ap_handle_m2_setup_message(m2ap_eNB_instance_t *instance_p, m2ap_eNB_data_t *eNB_desc_p, int sctp_shutdown);

int m2ap_eNB_handle_message(instance_t instance,
                            sctp_assoc_t assoc_id,
                            int32_t stream,
                            const uint8_t *const data,
                            const uint32_t data_length);

//int m2ap_handle_message(instance_t instance, sctp_assoc_t assoc_id, int32_t stream,
                            //const uint8_t * const data, const uint32_t data_length);


#endif /* M2AP_ENB_HANDLERS_H_ */
