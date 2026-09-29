/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef E1AP_INTERFACE_MANAGEMENT_H_
#define E1AP_INTERFACE_MANAGEMENT_H_

#include <stdbool.h>
#include "openair2/COMMON/e1ap_messages_types.h"
#include "common/platform_types.h"

/* GNB-CU-UP E1 Setup Request */
struct E1AP_E1AP_PDU *encode_e1ap_cuup_setup_request(const e1ap_setup_req_t *msg);
bool decode_e1ap_cuup_setup_request(const struct E1AP_E1AP_PDU *pdu, e1ap_setup_req_t *out);
e1ap_setup_req_t cp_e1ap_cuup_setup_request(const e1ap_setup_req_t *msg);
void free_e1ap_cuup_setup_request(e1ap_setup_req_t *msg);
bool eq_e1ap_cuup_setup_request(const e1ap_setup_req_t *a, const e1ap_setup_req_t *b);

/* GNB-CU-UP E1 Setup Response */
struct E1AP_E1AP_PDU *encode_e1ap_cuup_setup_response(const e1ap_setup_resp_t *msg);
bool decode_e1ap_cuup_setup_response(const struct E1AP_E1AP_PDU *pdu, e1ap_setup_resp_t *out);
e1ap_setup_resp_t cp_e1ap_cuup_setup_response(const e1ap_setup_resp_t *msg);
void free_e1ap_cuup_setup_response(e1ap_setup_resp_t *msg);
bool eq_e1ap_cuup_setup_response(const e1ap_setup_resp_t *a, const e1ap_setup_resp_t *b);

/* GNB-CU-UP E1 Setup Failure */
struct E1AP_E1AP_PDU *encode_e1ap_cuup_setup_failure(const e1ap_setup_fail_t *msg);
bool decode_e1ap_cuup_setup_failure(const struct E1AP_E1AP_PDU *pdu, e1ap_setup_fail_t *out);
e1ap_setup_fail_t cp_e1ap_cuup_setup_failure(const e1ap_setup_fail_t *msg);
void free_e1ap_cuup_setup_failure(e1ap_setup_fail_t *msg);
bool eq_e1ap_cuup_setup_failure(const e1ap_setup_fail_t *a, const e1ap_setup_fail_t *b);

#endif /* E1AP_INTERFACE_MANAGEMENT_H_ */
