/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m3ap tasks for MME
 */

#include <stdio.h>
#include <stdint.h>

/** @defgroup _m3ap_impl_ M3AP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

#ifndef M3AP_MME_H_
#define M3AP_MME_H_

#include "m3ap_MME_defs.h"


int m3ap_MME_init_sctp (m3ap_MME_instance_t *instance_p,
                        net_ip_address_t    *local_ip_addr,
                        uint32_t enb_port_for_M3C);

void *m3ap_MME_task(void *arg);

int is_m3ap_MME_enabled(void);

#endif /* M3AP_MCE_H_ */

/**
 * @}
 */
