/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef XNAP_GNB_INTERFACE_MANAGEMENT_H_
#define XNAP_GNB_INTERFACE_MANAGEMENT_H_

#include <stdbool.h>
#include "xnap_messages_types.h"
typedef struct XNAP_XnAP_PDU XNAP_XnAP_PDU_t;

XNAP_XnAP_PDU_t *encode_xn_setup_request(const xnap_setup_req_t *req);
XNAP_XnAP_PDU_t *encode_xn_setup_response(const xnap_setup_resp_t *resp);
XNAP_XnAP_PDU_t *encode_xn_setup_failure(const xnap_setup_failure_t *fail);

bool decode_xn_setup_request(xnap_setup_req_t *req, const XNAP_XnAP_PDU_t *pdu);
bool decode_xn_setup_response(xnap_setup_resp_t *out, const XNAP_XnAP_PDU_t *pdu);
bool decode_xn_setup_failure(xnap_setup_failure_t *out, const XNAP_XnAP_PDU_t *pdu);

bool eq_xnap_setup_request(const xnap_setup_req_t *a, const xnap_setup_req_t *b);
bool eq_xnap_setup_response(const xnap_setup_resp_t *a, const xnap_setup_resp_t *b);
bool eq_xnap_setup_failure(const xnap_setup_failure_t *a, const xnap_setup_failure_t *b);

void free_xnap_setup_request(const xnap_setup_req_t *msg);
void free_xnap_setup_response(const xnap_setup_resp_t *msg);
void free_xnap_setup_failure(xnap_setup_failure_t *msg);

#endif /* XNAP_GNB_INTERFACE_MANAGEMENT_H_ */
