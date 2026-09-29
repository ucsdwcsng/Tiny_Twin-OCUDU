/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m2ap handler procedures for MCE
 */

#ifndef __M2AP_MCE_HANDLERS__H__
#define __M2AP_MCE_HANDLERS__H__

#include "m2ap_MCE_defs.h"

//void m2ap_handle_m2_setup_message(m2ap_eNB_instance_t *instance_p, m2ap_eNB_data_t *eNB_desc_p, int sctp_shutdown);

int m2ap_MCE_handle_message(instance_t instance,
                            sctp_assoc_t assoc_id,
                            int32_t stream,
                            const uint8_t *const data,
                            const uint32_t data_length);

// int m2ap_handle_message(instance_t instance, sctp_assoc_t assoc_id, int32_t stream,
// const uint8_t * const data, const uint32_t data_length);

#endif /* __M2AP_MCE_HANDLERS__H__ */
