/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "assertions.h"
#include "intertask_interface.h"
#include "esm_proc.h"

#ifndef NAS_ITTI_MESSAGING_H_
#define NAS_ITTI_MESSAGING_H_

int nas_itti_plain_msg(
  const char *buffer,
  const nas_message_t *msg,
  const int lengthP,
  const int instance);

int nas_itti_protected_msg(const char *buffer, const nas_message_t *msg, const int lengthP, const int instance);

int nas_itti_kenb_refresh_req(const uint8_t kenb[32], int user_id);

int nas_itti_cell_info_req(const plmn_t plmnID, const uint8_t rat, int user_id);

int nas_itti_nas_establish_req(as_cause_t cause,
                               as_call_type_t type,
                               as_stmsi_t s_tmsi,
                               plmn_t plmnID,
                               uint8_t *data_pP,
                               uint32_t lengthP,
                               int user_id);

int nas_itti_ul_data_req(const uint32_t ue_idP, void *const data_pP, const uint32_t lengthP, int user_id);

int nas_itti_rab_establish_rsp(const as_stmsi_t s_tmsi, const as_rab_id_t rabID, const nas_error_code_t errCode, int user_id);
#endif /* NAS_ITTI_MESSAGING_H_ */
