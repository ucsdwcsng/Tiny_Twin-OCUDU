/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef S1AP_ENB_HANDLERS_H_
#define S1AP_ENB_HANDLERS_H_

void s1ap_handle_s1_setup_message(s1ap_eNB_mme_data_t *mme_desc_p, int sctp_shutdown);

int s1ap_eNB_handle_message(sctp_assoc_t assoc_id, int32_t stream, const uint8_t *const data, const uint32_t data_length);

#endif /* S1AP_ENB_HANDLERS_H_ */
