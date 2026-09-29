/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m3ap handler procedures for MME
 */

#ifndef M3AP_MME_HANDLERS_H_
#define M3AP_MME_HANDLERS_H_

#include "m3ap_MME_defs.h"

//void m3ap_handle_m2_setup_message(m2ap_eNB_instance_t *instance_p, m2ap_eNB_data_t *eNB_desc_p, int sctp_shutdown);

//int m3ap_eNB_handle_message(instance_t instance, sctp_assoc_t assoc_id, int32_t stream,
                            //const uint8_t * const data, const uint32_t data_length);

int m3ap_MME_handle_message(instance_t instance,
                            sctp_assoc_t assoc_id,
                            int32_t stream,
                            const uint8_t *const data,
                            const uint32_t data_length);

#endif /* M3AP_MME_HANDLERS_H_ */
