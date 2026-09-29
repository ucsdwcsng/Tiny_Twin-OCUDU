/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief m3ap tasks for MME
 */

#ifndef M3AP_MCE_ITTI_MESSAGING_H_
#define M3AP_MCE_ITTI_MESSAGING_H_

void m3ap_MCE_itti_send_sctp_data_req(instance_t instance,
                                      sctp_assoc_t assoc_id,
                                      uint8_t *buffer,
                                      uint32_t buffer_length,
                                      uint16_t stream);

void m3ap_MCE_itti_send_sctp_close_association(instance_t instance, sctp_assoc_t assoc_id);

void m3ap_MME_itti_send_sctp_data_req(instance_t instance,
                                      sctp_assoc_t assoc_id,
                                      uint8_t *buffer,
                                      uint32_t buffer_length,
                                      uint16_t stream);

void m3ap_MME_itti_send_sctp_close_association(instance_t instance, sctp_assoc_t assoc_id);




#endif /* M3AP_MCE_ITTI_MESSAGING_H_ */
