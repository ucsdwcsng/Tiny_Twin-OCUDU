/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <stdint.h>

#ifndef NRPPA_GNB_H_
#define NRPPA_GNB_H_

void nrppa_gNB_init(void);
void *nrppa_gNB_process_itti_msg(void *notUsed);
void *nrppa_gNB_task(void *arg);

#endif /* NRPPA_GNB_H_ */
