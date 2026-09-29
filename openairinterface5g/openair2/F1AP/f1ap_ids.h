/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/* "standalone" module to store a "secondary" UE ID for each UE in DU/CU.
 * Separate from the rest of F1, as it is also relevant for monolithic. */

#ifndef F1AP_IDS_H_
#define F1AP_IDS_H_

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>

typedef struct f1_ue_data_t {
  uint32_t secondary_ue;
  sctp_assoc_t e1_assoc_id;
  sctp_assoc_t du_assoc_id; // only used at the CU
} f1_ue_data_t;

void cu_init_f1_ue_data(void);
bool cu_add_f1_ue_data(uint32_t ue_id, const f1_ue_data_t *data);
bool cu_exists_f1_ue_data(uint32_t ue_id);
bool cu_update_f1_ue_data(uint32_t ue_id, const f1_ue_data_t *data);
f1_ue_data_t cu_get_f1_ue_data(uint32_t ue_id);
bool cu_remove_f1_ue_data(uint32_t ue_id);

void du_init_f1_ue_data(void);
bool du_add_f1_ue_data(uint32_t ue_id, const f1_ue_data_t *data);
bool du_exists_f1_ue_data(uint32_t ue_id);
f1_ue_data_t du_get_f1_ue_data(uint32_t ue_id);
bool du_remove_f1_ue_data(uint32_t ue_id);

#endif /* F1AP_IDS_H_ */
