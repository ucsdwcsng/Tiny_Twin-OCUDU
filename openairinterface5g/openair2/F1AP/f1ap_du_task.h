/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef F1AP_DU_TASK_H_
#define F1AP_DU_TASK_H_
#include "openair2/COMMON/f1ap_messages_types.h"

void du_task_send_sctp_association_req(instance_t instance, f1ap_net_config_t *du_inst);
void du_task_handle_sctp_association_resp(instance_t instance, sctp_new_association_resp_t *sctp_new_association_resp);
void du_task_handle_sctp_data_ind(instance_t instance, sctp_data_ind_t *sctp_data_ind);
void *F1AP_DU_task(void *arg);

#endif /* F1AP_DU_TASK_H_ */
