/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m3ap tasks for eNB
 */

#include <stdio.h>
#include <stdint.h>

/** @defgroup _m3ap_impl_ M3AP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

#ifndef M3AP_MCE_H_
#define M3AP_MCE_H_

#include "m3ap_MCE_defs.h"


int m3ap_MCE_init_sctp (m3ap_MCE_instance_t *instance_p,
                        net_ip_address_t    *local_ip_addr,
                        uint32_t mce_port_for_M3C);

void *m3ap_MCE_task(void *arg);

int is_m3ap_MCE_enabled(void);

#endif /* M3AP_H_ */

/**
 * @}
 */
