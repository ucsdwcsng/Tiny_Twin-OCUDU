/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "f1ap_common.h"
#include "f1ap_encoder.h"
#include "f1ap_itti_messaging.h"
#include "f1ap_cu_paging.h"
#include "lib/f1ap_paging.h"
#include "common/utils/ds/byte_array.h"

int CU_send_Paging(sctp_assoc_t assoc_id, const f1ap_paging_t *paging)
{
  F1AP_F1AP_PDU_t *pdu = encode_f1ap_paging(paging);
  if (pdu == NULL) {
    LOG_E(F1AP, "Failed to encode F1 Paging\n");
    return -1;
  }

  byte_array_t ba = {0};
  /* encode */
  if (f1ap_encode_pdu(pdu, &ba.buf, (uint32_t *)&ba.len) < 0) {
    LOG_E(F1AP, "Failed to encode F1 Paging PDU\n");
    ASN_STRUCT_FREE(asn_DEF_F1AP_F1AP_PDU, pdu);
    return -1;
  }
  ASN_STRUCT_FREE(asn_DEF_F1AP_F1AP_PDU, pdu);
  /* Ownership of ba.buf is transferred to SCTP task; do not free here. */
  f1ap_itti_send_sctp_data_req(assoc_id, ba.buf, ba.len);
  return 0;
}
