/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "f1ap_common.h"
#include "f1ap_itti_messaging.h"

void f1ap_itti_send_sctp_data_req(sctp_assoc_t assoc_id, uint8_t *buffer, uint32_t buffer_length)
{
  instance_t instance = 0; // we have only one instance
  MessageDef *message_p = itti_alloc_new_message(TASK_CU_F1, instance, SCTP_DATA_REQ);
  sctp_data_req_t *sctp_data_req = &message_p->ittiMsg.sctp_data_req;
  sctp_data_req->assoc_id = assoc_id;
  sctp_data_req->buffer = buffer;
  sctp_data_req->buffer_length = buffer_length;
  sctp_data_req->stream = 0;
  itti_send_msg_to_task(TASK_SCTP, instance, message_p);
}
