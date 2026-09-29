/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef FGS_SERVICE_ACCEPT_H
#define FGS_SERVICE_ACCEPT_H

#include <stdint.h>
#include "fgs_nas_utils.h"
#include "common/utils/ds/byte_array.h"
#include "fgmm_lib.h"

typedef struct {
  uint8_t pdu_session_id;
  uint8_t cause;
} pdu_error_cause_t;

typedef struct {
  // PDU Session Status (Optional)
  uint8_t psi_status[MAX_NUM_PSI]; // index 0 is spare
  bool has_psi_status;

  // PDU Session Reactivation Result (Optional)
  uint8_t psi_res[MAX_NUM_PSI]; // index 0 is spare
  bool has_psi_res;

  // PDU Session Reactivation Result Error Cause (Optional)
  pdu_error_cause_t cause[MAX_NUM_PDU_ERRORS];
  uint16_t num_errors;

  // T3448 Value (Optional)
  gprs_timer_t *t3448;

  // EAP Message (Optional)
  byte_array_t eap_msg;
} fgs_service_accept_msg_t;

int decode_fgs_service_accept(fgs_service_accept_msg_t *msg, const byte_array_t *buffer);
int encode_fgs_service_accept(byte_array_t *buffer, const fgs_service_accept_msg_t *msg);
void free_fgs_service_accept(fgs_service_accept_msg_t *msg);
bool eq_service_accept(const fgs_service_accept_msg_t *a, const fgs_service_accept_msg_t *b);

#endif /* FGS_SERVICE_ACCEPT_H */
