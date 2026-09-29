/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m3ap handler procedures for eNB
 */

#ifndef __M3AP_MCE_HANDLER__H__
#define __M3AP_MCE_HANDLER__H__

#include "m2ap_MCE_defs.h"

//void m3ap_handle_m2_setup_message(m2ap_eNB_instance_t *instance_p, m2ap_eNB_data_t *eNB_desc_p, int sctp_shutdown);

//int m3ap_eNB_handle_message(instance_t instance, sctp_assoc_t assoc_id, int32_t stream,
                            //const uint8_t * const data, const uint32_t data_length);

int m3ap_MCE_handle_message(instance_t instance,
                            sctp_assoc_t assoc_id,
                            int32_t stream,
                            const uint8_t *const data,
                            const uint32_t data_length);

#endif /* __M3AP_MCE_HANDLER__H__ */
