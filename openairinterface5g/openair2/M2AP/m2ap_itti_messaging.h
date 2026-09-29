/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m2ap tasks for eNB
 */

#ifndef M2AP_ENB_ITTI_MESSAGING_H_
#define M2AP_ENB_ITTI_MESSAGING_H_

void m2ap_eNB_itti_send_sctp_data_req(instance_t instance,
                                      sctp_assoc_t assoc_id,
                                      uint8_t *buffer,
                                      uint32_t buffer_length,
                                      uint16_t stream);

void m2ap_eNB_itti_send_sctp_close_association(instance_t instance, sctp_assoc_t assoc_id);

void m2ap_MCE_itti_send_sctp_data_req(instance_t instance,
                                      sctp_assoc_t assoc_id,
                                      uint8_t *buffer,
                                      uint32_t buffer_length,
                                      uint16_t stream);

void m2ap_MCE_itti_send_sctp_close_association(instance_t instance, sctp_assoc_t assoc_id);




#endif /* M2AP_ENB_ITTI_MESSAGING_H_ */
