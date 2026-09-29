/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief rrc M2AP procedures for eNB
 */

#ifndef RRC_ENB_M2AP_H_
#define RRC_ENB_M2AP_H_


int
rrc_eNB_process_M2AP_SETUP_RESP(
  const protocol_ctxt_t *const ctxt_pP,
  int CC_id,
  instance_t instance,
  const m2ap_setup_resp_t *const m2ap_setup_resp
);

int
rrc_eNB_process_M2AP_MBMS_SCHEDULING_INFORMATION(
  const protocol_ctxt_t *const ctxt_pP,
  int CC_id,
  instance_t instance,
  const m2ap_mbms_scheduling_information_t *const m2ap_mbms_scheduling_information
);

int
rrc_eNB_process_M2AP_MBMS_SESSION_START_REQ(
  const protocol_ctxt_t *const ctxt_pP,
  int CC_id,
  instance_t instance,
  const m2ap_session_start_req_t *const m2ap_session_start_req
);

int
rrc_eNB_process_M2AP_MBMS_SESSION_STOP_REQ(
  const protocol_ctxt_t *const ctxt_pP,
  const m2ap_session_stop_req_t *const m2ap_session_stop_req
);

int
rrc_eNB_process_M2AP_RESET(
  const protocol_ctxt_t *const ctxt_pP,
  const m2ap_reset_t *const m2ap_reset
);

int
rrc_eNB_process_M2AP_ENB_CONFIGURATION_UPDATE_ACK(
  const protocol_ctxt_t *const ctxt_pP,
  const m2ap_enb_configuration_update_ack_t *const m2ap_enb_configuration_update_ack
);

int
rrc_eNB_process_M2AP_ERROR_INDICATION(
  const protocol_ctxt_t *const ctxt_pP,
  const m2ap_error_indication_t *const m2ap_error_indication
);

int
rrc_eNB_process_M2AP_MBMS_SERVICE_COUNTING_REQ(
  const protocol_ctxt_t *const ctxt_pP,
  const m2ap_mbms_service_counting_req_t *const m2ap_mbms_service_counting_req
);

int
rrc_eNB_process_M2AP_MCE_CONFIGURATION_UPDATE(
  const protocol_ctxt_t *const ctxt_pP,
  const m2ap_mce_configuration_update_t *const m2ap_mce_configuration_update
);


void rrc_eNB_send_M2AP_MBMS_SCHEDULING_INFORMATION_RESP(
  const protocol_ctxt_t    *const ctxt_pP
  //,const rrc_eNB_mbms_context_t *const rrc_eNB_mbms_context
);

void rrc_eNB_send_M2AP_MBMS_SESSION_START_RESP(
  const protocol_ctxt_t    *const ctxt_pP
  //,const rrc_eNB_mbms_context_t *const rrc_eNB_mbms_context
);

void rrc_eNB_send_M2AP_MBMS_SESSION_STOP_RESP(
  const protocol_ctxt_t    *const ctxt_pP
  //,const rrc_eNB_mbms_context_t *const rrc_eNB_mbms_context
);

void rrc_eNB_send_M2AP_MBMS_SESSION_UPDATE_RESP(
  const protocol_ctxt_t    *const ctxt_pP
  //,const rrc_eNB_mbms_context_t *const rrc_eNB_mbms_context
);

#endif /* RRC_ENB_M2AP_H_ */
