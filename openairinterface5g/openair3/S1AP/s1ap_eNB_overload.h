/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef S1AP_ENB_OVERLOAD_H_
#define S1AP_ENB_OVERLOAD_H_

/**
 * \brief Handle an overload start message
 **/
// int s1ap_eNB_handle_overload_start(eNB_mme_desc_t *eNB_desc_p,
//                                    sctp_queue_item_t *packet_p,
//                                    struct s1ap_message_s *message_p);
int s1ap_eNB_handle_overload_start(sctp_assoc_t assoc_id, uint32_t stream, S1AP_S1AP_PDU_t *pdu);

/**
 * \brief Handle an overload stop message
 **/
// int s1ap_eNB_handle_overload_stop(eNB_mme_desc_t *eNB_desc_p,
//                                   sctp_queue_item_t *packet_p,
//                                   struct s1ap_message_s *message_p);
int s1ap_eNB_handle_overload_stop(sctp_assoc_t assoc_id, uint32_t stream, S1AP_S1AP_PDU_t *pdu);

#endif /* S1AP_ENB_OVERLOAD_H_ */
