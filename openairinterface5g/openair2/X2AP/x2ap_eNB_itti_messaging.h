/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief x2ap tasks for eNB
 */

#ifndef X2AP_ENB_ITTI_MESSAGING_H_
#define X2AP_ENB_ITTI_MESSAGING_H_

void x2ap_eNB_itti_send_sctp_data_req(instance_t instance,
                                      sctp_assoc_t assoc_id,
                                      uint8_t *buffer,
                                      uint32_t buffer_length,
                                      uint16_t stream);

void x2ap_eNB_itti_send_sctp_close_association(instance_t instance, sctp_assoc_t assoc_id);


#endif /* X2AP_ENB_ITTI_MESSAGING_H_ */
