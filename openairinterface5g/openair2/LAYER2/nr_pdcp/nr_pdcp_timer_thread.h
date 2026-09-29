/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef _NR_PDCP_TIMER_THREAD_H_
#define _NR_PDCP_TIMER_THREAD_H_

#include "nr_pdcp_ue_manager.h"

#include <stdint.h>

void nr_pdcp_init_timer_thread(nr_pdcp_ue_manager_t *nr_pdcp_ue_manager);
void nr_pdcp_ms_tick(void);
uint64_t nr_pdcp_current_time(void);

#endif /* _NR_PDCP_TIMER_THREAD_H_ */
