/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef F1AP_CU_TASK_H_
#define F1AP_CU_TASK_H_

#include <stdint.h>
#include "common/ngran_types.h"

typedef struct f1ap_cu_conf {
  ngran_node_t type;
  char *bind_addr;
  // if type == ngran_gNB_CU, will create GTP
  uint16_t local_f1u_port;
  uint16_t remote_f1u_port;
} f1ap_cu_conf_t;

void *F1AP_CU_task(void *arg);

#endif /* F1AP_CU_TASK_H_ */
