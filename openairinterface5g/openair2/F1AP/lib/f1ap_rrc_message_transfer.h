/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef F1AP_RRC_MESSAGE_TRANSFER_H_
#define F1AP_RRC_MESSAGE_TRANSFER_H_

#include <stdbool.h>
#include "f1ap_messages_types.h"

struct F1AP_F1AP_PDU;

struct F1AP_F1AP_PDU *encode_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_message_t *msg);
bool decode_initial_ul_rrc_message_transfer(const struct F1AP_F1AP_PDU *pdu, f1ap_initial_ul_rrc_message_t *out);
f1ap_initial_ul_rrc_message_t cp_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_message_t *msg);
bool eq_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_message_t *a, const f1ap_initial_ul_rrc_message_t *b);
void free_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_message_t *msg);

/* DL RRC Message transfer */
struct F1AP_F1AP_PDU *encode_dl_rrc_message_transfer(const f1ap_dl_rrc_message_t *msg);
bool decode_dl_rrc_message_transfer(const struct F1AP_F1AP_PDU *pdu, f1ap_dl_rrc_message_t *out);
f1ap_dl_rrc_message_t cp_dl_rrc_message_transfer(const f1ap_dl_rrc_message_t *msg);
bool eq_dl_rrc_message_transfer(const f1ap_dl_rrc_message_t *a, const f1ap_dl_rrc_message_t *b);
void free_dl_rrc_message_transfer(const f1ap_dl_rrc_message_t *msg);

/* UL RRC Message transfer */
struct F1AP_F1AP_PDU *encode_ul_rrc_message_transfer(const f1ap_ul_rrc_message_t *msg);
bool decode_ul_rrc_message_transfer(const struct F1AP_F1AP_PDU *pdu, f1ap_ul_rrc_message_t *out);
f1ap_ul_rrc_message_t cp_ul_rrc_message_transfer(const f1ap_ul_rrc_message_t *msg);
bool eq_ul_rrc_message_transfer(const f1ap_ul_rrc_message_t *a, const f1ap_ul_rrc_message_t *b);
void free_ul_rrc_message_transfer(const f1ap_ul_rrc_message_t *msg);

#endif /* F1AP_RRC_MESSAGE_TRANSFER_H_ */
