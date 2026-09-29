/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m3ap interface management for MME
 */

#ifndef M3AP_MME_INTERFACE_MANAGEMENT_H_
#define M3AP_MME_INTERFACE_MANAGEMENT_H_
/*
 * MBMS Session start
 */
int MME_send_MBMS_SESSION_START_REQUEST(instance_t instance,
                                        /*sctp_assoc_t assoc_id,*/
                                        m3ap_session_start_req_t *m3ap_session_start_req);
int MME_handle_MBMS_SESSION_START_RESPONSE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, M3AP_M3AP_PDU_t *pdu);

int MME_handle_MBMS_SESSION_START_FAILURE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, M3AP_M3AP_PDU_t *pdu);
/*
 * MBMS Session stop
 */
int MME_send_MBMS_SESSION_STOP_REQUEST(instance_t instance,
                                m3ap_session_stop_req_t* m3ap_session_stop_req);

int MME_handle_MBMS_SESSION_STOP_RESPONSE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, M3AP_M3AP_PDU_t *pdu);

/*
 * Update 
 */
int MME_handle_MBMS_SESSION_UPDATE_RESPONSE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, M3AP_M3AP_PDU_t *pdu);

int MME_handle_MBMS_SESSION_UPDATE_FAILURE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, M3AP_M3AP_PDU_t *pdu);

/*
 * Reset
 */
int MME_send_RESET(instance_t instance, m3ap_reset_t * m3ap_reset);

int MME_handle_RESET_ACKKNOWLEDGE(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, M3AP_M3AP_PDU_t *pdu);

int MME_handle_RESET(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, M3AP_M3AP_PDU_t *pdu);
int MME_send_RESET_ACKNOWLEDGE(instance_t instance, M3AP_ResetAcknowledge_t *ResetAcknowledge);

/*
 * M3AP Setup
 */
int MME_handle_M3_SETUP_REQUEST(instance_t instance, sctp_assoc_t assoc_id, uint32_t stream, M3AP_M3AP_PDU_t *pdu);

int MME_send_M3_SETUP_RESPONSE(instance_t instance, /*sctp_assoc_t assoc_id,*/ m3ap_setup_resp_t *m3ap_setup_resp);

int MME_send_M3_SETUP_FAILURE(instance_t instance, /*sctp_assoc_t assoc_id*/ m3ap_setup_failure_t *m3ap_setup_failure);

#endif /* M3AP_MME_INTERFACE_MANAGEMENT_H_ */



