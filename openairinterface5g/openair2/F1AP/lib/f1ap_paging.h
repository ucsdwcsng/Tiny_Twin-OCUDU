/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef F1AP_PAGING_H_
#define F1AP_PAGING_H_

#include <stdbool.h>
#include "f1ap_messages_types.h"

struct F1AP_F1AP_PDU;

/* F1 Paging */
struct F1AP_F1AP_PDU *encode_f1ap_paging(const f1ap_paging_t *msg);
bool decode_f1ap_paging(f1ap_paging_t *out, const struct F1AP_F1AP_PDU *pdu);
void free_f1ap_paging(f1ap_paging_t *msg);
bool eq_f1ap_paging(const f1ap_paging_t *a, const f1ap_paging_t *b);
f1ap_paging_t cp_f1ap_paging(const f1ap_paging_t *orig);

#endif /* F1AP_PAGING_H_ */
