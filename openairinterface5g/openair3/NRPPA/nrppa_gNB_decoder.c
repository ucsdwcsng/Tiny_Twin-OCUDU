/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "assertions.h"
#include "intertask_interface.h"
#include "nrppa_common.h"
#include "nrppa_gNB_decoder.h"

static int nrppa_gNB_decode_initiating_message(NRPPA_NRPPA_PDU_t *pdu)
{
  asn_encode_to_new_buffer_result_t res = {NULL, {0, NULL, NULL}};
  DevAssert(pdu != NULL);

  switch (pdu->choice.initiatingMessage->procedureCode) {
    case NRPPA_ProcedureCode_id_tRPInformationExchange: // TRPInformationRequest
      res = asn_encode_to_new_buffer(NULL, ATS_CANONICAL_XER, &asn_DEF_NRPPA_NRPPA_PDU, pdu);
      LOG_I(NRPPA, "TRP Information Request initiating message\n");
      free(res.buffer);
      break;
    case NRPPA_ProcedureCode_id_positioningInformationExchange: // PositioningInformationRequest
      res = asn_encode_to_new_buffer(NULL, ATS_CANONICAL_XER, &asn_DEF_NRPPA_NRPPA_PDU, pdu);
      LOG_I(NRPPA, "Positioning Information Request initiating message\n");
      free(res.buffer);
      break;
    case NRPPA_ProcedureCode_id_positioningActivation:
      res = asn_encode_to_new_buffer(NULL, ATS_CANONICAL_XER, &asn_DEF_NRPPA_NRPPA_PDU, pdu);
      LOG_I(NRPPA, "Positioning Activation initiating message\n");
      free(res.buffer);
      break;
    case NRPPA_ProcedureCode_id_Measurement:
      res = asn_encode_to_new_buffer(NULL, ATS_CANONICAL_XER, &asn_DEF_NRPPA_NRPPA_PDU, pdu);
      LOG_I(NRPPA, "Positioning Measurement initiating message\n");
      free(res.buffer);
      break;
    default:
      AssertFatal(false, "Unknown procedure ID (%d) for initiating message\n", (int)pdu->choice.initiatingMessage->procedureCode);
      return -1;
  }
  return 0;
}

static int nrppa_gNB_decode_successful_outcome(NRPPA_NRPPA_PDU_t *pdu)
{
  asn_encode_to_new_buffer_result_t res = {NULL, {0, NULL, NULL}};
  DevAssert(pdu != NULL);

  switch (pdu->choice.successfulOutcome->procedureCode) {
    case NRPPA_ProcedureCode_id_tRPInformationExchange:
      res = asn_encode_to_new_buffer(NULL, ATS_CANONICAL_XER, &asn_DEF_NRPPA_NRPPA_PDU, pdu);
      LOG_I(NRPPA, "TRP Information Response successfull outcome message\n");
      free(res.buffer);
      break;

    default:
      AssertFatal(false,
                  "Unknown procedure ID (%d) for successfull outcome message\n",
                  (int)pdu->choice.initiatingMessage->procedureCode);
      return -1;
  }
  return 0;
}

static int nrppa_gNB_decode_unsuccessful_outcome(NRPPA_NRPPA_PDU_t *pdu)
{
  asn_encode_to_new_buffer_result_t res = {NULL, {0, NULL, NULL}};
  DevAssert(pdu != NULL);

  switch (pdu->choice.unsuccessfulOutcome->procedureCode) {
    case NRPPA_ProcedureCode_id_tRPInformationExchange:
      res = asn_encode_to_new_buffer(NULL, ATS_CANONICAL_XER, &asn_DEF_NRPPA_NRPPA_PDU, pdu);
      LOG_I(NRPPA, "TRP Information Failure unsuccessfull outcome message\n");
      free(res.buffer);
      break;

    default:
      AssertFatal(false,
                  "Unknown procedure ID (%d) for unsuccessfull outcome message\n",
                  (int)pdu->choice.initiatingMessage->procedureCode);
      return -1;
  }
  return 0;
}

int nrppa_gNB_decode_pdu(NRPPA_NRPPA_PDU_t *pdu, const uint8_t *const buffer, const uint32_t length)
{
  asn_dec_rval_t dec_ret;
  DevAssert(pdu != NULL);
  DevAssert(buffer != NULL);
  asn_codec_ctx_t st = {.max_stack_size = 100 * 1000}; // if we enable asn1c debug the stack size become large
  dec_ret = aper_decode(&st, &asn_DEF_NRPPA_NRPPA_PDU, (void **)&pdu, buffer, length, 0, 0);
  if (dec_ret.code != RC_OK) {
    LOG_E(NRPPA, "Failed to decode pdu\n");
    return -1;
  }

  switch (pdu->present) {
    case NRPPA_NRPPA_PDU_PR_initiatingMessage:
      return nrppa_gNB_decode_initiating_message(pdu);

    case NRPPA_NRPPA_PDU_PR_successfulOutcome:
      return nrppa_gNB_decode_successful_outcome(pdu);

    case NRPPA_NRPPA_PDU_PR_unsuccessfulOutcome:
      return nrppa_gNB_decode_unsuccessful_outcome(pdu);

    default:
      LOG_I(NRPPA, "Unknown presence (%d) or not implemented\n", (int)pdu->present);
      break;
  }

  return -1;
}
