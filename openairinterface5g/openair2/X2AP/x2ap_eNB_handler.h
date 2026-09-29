/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief x2ap handler procedures for eNB
 */

#ifndef X2AP_ENB_HANDLERS_H_
#define X2AP_ENB_HANDLERS_H_

#include "x2ap_eNB_defs.h"

void x2ap_handle_x2_setup_message(x2ap_eNB_instance_t *instance_p, x2ap_eNB_data_t *eNB_desc_p, int sctp_shutdown);

int x2ap_eNB_handle_message(instance_t instance,
                            sctp_assoc_t assoc_id,
                            int32_t stream,
                            const uint8_t *const data,
                            const uint32_t data_length);

#endif /* X2AP_ENB_HANDLERS_H_ */
