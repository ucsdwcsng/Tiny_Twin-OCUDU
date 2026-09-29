/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief x2ap tasks for eNB
 */

#include <stdio.h>
#include <stdint.h>

/** @defgroup _x2ap_impl_ X2AP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

#ifndef X2AP_H_
#define X2AP_H_

#define X2AP_SCTP_PPID   (27)    ///< X2AP SCTP Payload Protocol Identifier (PPID)
#include "x2ap_eNB_defs.h"

int x2ap_eNB_init_sctp (x2ap_eNB_instance_t *instance_p,
                        net_ip_address_t    *local_ip_addr,
                        uint32_t enb_port_for_X2C);

void *x2ap_task(void *arg);

int is_x2ap_enabled(void);

#endif /* X2AP_H_ */

/**
 * @}
 */
