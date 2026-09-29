/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "nrppa_common.h"
#include "nrppa_gNB_encoder.h"

static inline int nrppa_gNB_encode_initiating(NRPPA_NRPPA_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
{
  DevAssert(pdu != NULL);

  const NRPPA_ProcedureCode_t tmp[] = {NRPPA_ProcedureCode_id_positioningInformationExchange,
                                       NRPPA_ProcedureCode_id_positioningActivation,
                                       NRPPA_ProcedureCode_id_positioningInformationUpdate,
                                       NRPPA_ProcedureCode_id_positioningDeactivation,
                                       NRPPA_ProcedureCode_id_tRPInformationExchange,
                                       NRPPA_ProcedureCode_id_Measurement,
                                       NRPPA_ProcedureCode_id_MeasurementReport,
                                       NRPPA_ProcedureCode_id_MeasurementFailureIndication,
                                       NRPPA_ProcedureCode_id_MeasurementAbort,
                                       NRPPA_ProcedureCode_id_MeasurementUpdate};

  int i;
  for (i = 0; i < sizeofArray(tmp); i++)
    if (pdu->choice.initiatingMessage->procedureCode == tmp[i])
      break;
  if (i == sizeofArray(tmp)) {
    LOG_E(NRPPA, "Unknown procedure ID (%d) for initiating message\n", (int)pdu->choice.initiatingMessage->procedureCode);
    return -1;
  }

  asn_encode_to_new_buffer_result_t res = asn_encode_to_new_buffer(NULL, ATS_ALIGNED_CANONICAL_PER, &asn_DEF_NRPPA_NRPPA_PDU, pdu);
  AssertFatal(res.result.encoded > 0, "failed to encode NRPPA msg\n");
  *buffer = res.buffer;
  *len = res.result.encoded;
  return 0;
}

static inline int nrppa_gNB_encode_successfull_outcome(NRPPA_NRPPA_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
{
  DevAssert(pdu != NULL);
  const NRPPA_ProcedureCode_t tmp[] = {NRPPA_ProcedureCode_id_positioningInformationExchange,
                                       NRPPA_ProcedureCode_id_positioningActivation,
                                       NRPPA_ProcedureCode_id_tRPInformationExchange,
                                       NRPPA_ProcedureCode_id_Measurement};
  int i;
  for (i = 0; i < sizeofArray(tmp); i++)
    if (pdu->choice.successfulOutcome->procedureCode == tmp[i])
      break;
  if (i == sizeofArray(tmp)) {
    LOG_E(NRPPA, "Unknown procedure ID (%ld) for successfull outcome message\n", pdu->choice.successfulOutcome->procedureCode);
    return -1;
  }
  // xer_fprint(stdout, &asn_DEF_NRPPA_NRPPA_PDU, pdu);
  asn_encode_to_new_buffer_result_t res = asn_encode_to_new_buffer(NULL, ATS_ALIGNED_CANONICAL_PER, &asn_DEF_NRPPA_NRPPA_PDU, pdu);
  AssertFatal(res.result.encoded > 0, "failed to encode NRPPA msg\n");
  *buffer = res.buffer;
  *len = res.result.encoded;
  return 0;
}

static inline int nrppa_gNB_encode_unsuccessfull_outcome(NRPPA_NRPPA_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
{
  DevAssert(pdu != NULL);

  const NRPPA_ProcedureCode_t tmp[] = {NRPPA_ProcedureCode_id_positioningInformationExchange,
                                       NRPPA_ProcedureCode_id_positioningActivation,
                                       NRPPA_ProcedureCode_id_tRPInformationExchange,
                                       NRPPA_ProcedureCode_id_Measurement};

  int i;
  for (i = 0; i < sizeofArray(tmp); i++)
    if (pdu->choice.unsuccessfulOutcome->procedureCode == tmp[i])
      break;
  if (i == sizeofArray(tmp)) {
    LOG_W(NRPPA, "Unknown procedure ID (%ld) for unsuccessfull outcome message\n", pdu->choice.unsuccessfulOutcome->procedureCode);
    return -1;
  }

  asn_encode_to_new_buffer_result_t res = asn_encode_to_new_buffer(NULL, ATS_ALIGNED_CANONICAL_PER, &asn_DEF_NRPPA_NRPPA_PDU, pdu);
  AssertFatal(res.result.encoded > 0, "failed to encode NRPPA msg\n");
  *buffer = res.buffer;
  *len = res.result.encoded;
  return 0;
}

int nrppa_gNB_encode_pdu(NRPPA_NRPPA_PDU_t *pdu, uint8_t **buffer, uint32_t *len)
{
  DevAssert(pdu != NULL);
  DevAssert(buffer != NULL);
  DevAssert(len != NULL);

  char errbuf[1024];
  size_t errlen = sizeof(errbuf);
  int ret = asn_check_constraints(&asn_DEF_NRPPA_NRPPA_PDU, pdu, errbuf, &errlen);
  AssertFatal(ret == 0, "asn_check_constraints() failed: %s\n", errbuf);

  // xer_fprint(stdout, &asn_DEF_NRPPA_NRPPA_PDU, (void *)pdu);

  switch (pdu->present) {
    case NRPPA_NRPPA_PDU_PR_initiatingMessage:
      ret = nrppa_gNB_encode_initiating(pdu, buffer, len);
      break;

    case NRPPA_NRPPA_PDU_PR_successfulOutcome:
      ret = nrppa_gNB_encode_successfull_outcome(pdu, buffer, len);
      break;

    case NRPPA_NRPPA_PDU_PR_unsuccessfulOutcome:
      ret = nrppa_gNB_encode_unsuccessfull_outcome(pdu, buffer, len);
      break;

    default:
      LOG_E(NRPPA, "Unknown message outcome (%d) or not implemented", (int)pdu->present);
      return -1;
  }
  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NRPPA_NRPPA_PDU, pdu);
  return ret;
}
