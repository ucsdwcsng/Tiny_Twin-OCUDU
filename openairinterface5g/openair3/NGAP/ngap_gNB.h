/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */
 
/*!
 * \brief NGAP gNB task
 * @ingroup _ngap
 */

#include <stdint.h>

/** @defgroup _ngap_impl_ NGAP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

#ifndef NGAP_GNB_H_
#define NGAP_GNB_H_

void *ngap_gNB_process_itti_msg(void*);
void  ngap_gNB_init(void);
void *ngap_gNB_task(void *arg);

uint32_t ngap_generate_gNB_id(void);

#endif /* NGAP_GNB_H_ */

/**
 * @}
 */
