/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m2ap tasks for MCE
 */

#include <stdio.h>
#include <stdint.h>

/** @defgroup _m2ap_impl_ M2AP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

#ifndef M2AP_MCE_H_
#define M2AP_MCE_H_

#include "m2ap_MCE_defs.h"


int m2ap_MCE_init_sctp (m2ap_MCE_instance_t *instance_p,
                        net_ip_address_t    *local_ip_addr,
                        uint32_t enb_port_for_M2C);

void *m2ap_MCE_task(void *arg);

int is_m2ap_MCE_enabled(void);

#endif /* M2AP_MCE_H_ */

/**
 * @}
 */
