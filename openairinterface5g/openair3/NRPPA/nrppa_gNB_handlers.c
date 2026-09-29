/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#include "intertask_interface.h"
#include "nrppa_gNB_handlers.h"
#include "nrppa_gNB_decoder.h"
#include "nrppa_common.h"
#include "nrppa_gNB_location_information_transfer.h"
#include "nrppa_gNB_measurement_information_transfer.h"

char *nrppa_direction2String(int nrppa_dir)
{
  static char *nrppa_direction_String[] = {
      "", /* Nothing */
      "Originating message", /* originating message */
      "Successfull outcome", /* successfull outcome */
      "UnSuccessfull outcome", /* successfull outcome */
  };
  return (nrppa_direction_String[nrppa_dir]);
}

/* Handlers matrix. Only gNB related procedure present here */
nrppa_message_decoded_callback nrppa_messages_callback[][3] = {
    {0, 0, 0}, // NRPPA_ProcedureCode_id_errorIndication        ((NRPPA_ProcedureCode_t)0)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_privateMessage ((NRPPA_ProcedureCode_t)1)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_e_CIDMeasurementInitiation     ((NRPPA_ProcedureCode_t)2)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_e_CIDMeasurementFailureIndication      ((NRPPA_ProcedureCode_t)3)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_e_CIDMeasurementReport ((NRPPA_ProcedureCode_t)4)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_e_CIDMeasurementTermination    ((NRPPA_ProcedureCode_t)5)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_oTDOAInformationExchange       ((NRPPA_ProcedureCode_t)6)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_assistanceInformationControl   ((NRPPA_ProcedureCode_t)7)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_assistanceInformationFeedback  ((NRPPA_ProcedureCode_t)8)
    {nrppa_gNB_handle_positioning_information_request,
     0,
     0}, // NRPPA_ProcedureCode_id_positioningInformationExchange ((NRPPA_ProcedureCode_t)9)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_positioningInformationUpdate   ((NRPPA_ProcedureCode_t)10)
    {nrppa_gNB_handle_measurement_request, 0, 0}, // NRPPA_ProcedureCode_id_Measurement ((NRPPA_ProcedureCode_t)11)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_MeasurementReport      ((NRPPA_ProcedureCode_t)12)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_MeasurementUpdate    ((NRPPA_ProcedureCode_t)13)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_MeasurementAbort     ((NRPPA_ProcedureCode_t)14)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_MeasurementFailureIndication   ((NRPPA_ProcedureCode_t)15)
    {nrppa_gNB_handle_trp_information_request, 0, 0}, // NRPPA_ProcedureCode_id_tRPInformationExchange   ((NRPPA_ProcedureCode_t)16)
    {nrppa_gNB_handle_positioning_activation_request,
     0,
     0}, // NRPPA_ProcedureCode_id_positioningActivation        ((NRPPA_ProcedureCode_t)17)
    {0, 0, 0}, // NRPPA_ProcedureCode_id_positioningDeactivation      ((NRPPA_ProcedureCode_t)18)
};

// Processing DOWNLINK UE ASSOCIATED NRPPA TRANSPORT (9.2.9.1 of TS 38.413 Version 16.0.0)
int nrppa_handle_downlink_ue_associated_nrppa_transport(instance_t instance, const ngap_downlink_ue_associated_nrppa_t *msg)
{
  LOG_I(NRPPA, "Handling DOWNLINKUEASSOCIATEDNRPPA TRANSPORT\n");
  uint8_t *const data = msg->nrppa_pdu.buf;
  const uint32_t data_length = msg->nrppa_pdu.len;
  NRPPA_NRPPA_PDU_t pdu;
  int ret;
  DevAssert(data != NULL);
  memset(&pdu, 0, sizeof(pdu));

  if (nrppa_gNB_decode_pdu(&pdu, data, data_length) < 0) {
    LOG_E(NRPPA, "Failed to decode PDU\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NRPPA_NRPPA_PDU, &pdu);
    return -1;
  }
  /* Checking procedure Code and direction of message*/
  if (pdu.choice.initiatingMessage->procedureCode >= sizeof(nrppa_messages_callback) / (3 * sizeof(nrppa_message_decoded_callback))
      || (pdu.present > NRPPA_NRPPA_PDU_PR_unsuccessfulOutcome)) {
    LOG_E(NRPPA,
          "Either procedureCode %ld or direction %d exceed expected\n",
          pdu.choice.initiatingMessage->procedureCode,
          pdu.present);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NRPPA_NRPPA_PDU, &pdu);
    return -1;
  }

  /* No handler present.
   * This can mean not implemented or no procedure for gNB (wrong direction).*/
  if (nrppa_messages_callback[pdu.choice.initiatingMessage->procedureCode][pdu.present - 1] == NULL) {
    LOG_E(NRPPA,
          "No handler for procedureCode %ld in %s\n",
          pdu.choice.initiatingMessage->procedureCode,
          nrppa_direction2String(pdu.present - 1));
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NRPPA_NRPPA_PDU, &pdu);
    return -1;
  }

  /* Calling the right handler*/
  nrppa_gnb_ue_info_t nrppa_msg_info;
  nrppa_msg_info.instance = instance;
  nrppa_msg_info.gNB_ue_ngap_id = msg->gNB_ue_ngap_id;
  nrppa_msg_info.amf_ue_ngap_id = msg->amf_ue_ngap_id;
  nrppa_msg_info.routing_id.buf = msg->routing_id.buf;
  nrppa_msg_info.routing_id.len = msg->routing_id.len;
  ret = (*nrppa_messages_callback[pdu.choice.initiatingMessage->procedureCode][pdu.present - 1])(&nrppa_msg_info, &pdu);

  if (msg->routing_id.buf) {
    free(msg->routing_id.buf);
  }

  if (msg->nrppa_pdu.buf) {
    free(msg->nrppa_pdu.buf);
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NRPPA_NRPPA_PDU, &pdu);
  return ret;
}

// Processing DOWNLINK NON UE ASSOCIATED NRPPA TRANSPORT (9.2.9.3 of TS 38.413 Version 16.0.0)
int nrppa_handle_downlink_non_ue_associated_nrppa_transport(instance_t instance, const ngap_downlink_non_ue_associated_nrppa_t *msg)
{
  LOG_I(NRPPA, "Handling DOWNLINKNONUEASSOCIATEDNRPPA TRANSPORT\n");
  uint8_t *const data = msg->nrppa_pdu.buf;
  const uint32_t data_length = msg->nrppa_pdu.len;
  NRPPA_NRPPA_PDU_t pdu;
  int ret;
  DevAssert(data != NULL);
  memset(&pdu, 0, sizeof(pdu));

  if (nrppa_gNB_decode_pdu(&pdu, data, data_length) < 0) {
    LOG_E(NRPPA, "Failed to decode Downlink Non UE Associated NRPPa PDU\n");
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NRPPA_NRPPA_PDU, &pdu);
    return -1;
  }

  /* Checking procedure Code and direction of message*/
  if (pdu.choice.initiatingMessage->procedureCode >= sizeof(nrppa_messages_callback) / (3 * sizeof(nrppa_message_decoded_callback))
      || (pdu.present > NRPPA_NRPPA_PDU_PR_unsuccessfulOutcome)) {
    LOG_E(NRPPA,
          "Either procedureCode %ld or direction %d exceed expected\n",
          pdu.choice.initiatingMessage->procedureCode,
          pdu.present);
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NRPPA_NRPPA_PDU, &pdu);
    return -1;
  }

  /* No handler present.
   * This can mean not implemented or no procedure for gNB (wrong direction).*/
  if (nrppa_messages_callback[pdu.choice.initiatingMessage->procedureCode][pdu.present - 1] == NULL) {
    LOG_E(NRPPA,
          "No handler for procedureCode %ld in %s\n",
          pdu.choice.initiatingMessage->procedureCode,
          nrppa_direction2String(pdu.present - 1));
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NRPPA_NRPPA_PDU, &pdu);
    return -1;
  }

  /* Calling the right handler*/
  nrppa_gnb_ue_info_t nrppa_msg_info;
  nrppa_msg_info.instance = instance;
  nrppa_msg_info.gNB_ue_ngap_id = 0; // non-ue associated
  nrppa_msg_info.amf_ue_ngap_id = 0; // non-ue associated
  nrppa_msg_info.routing_id.buf = msg->routing_id.buf;
  nrppa_msg_info.routing_id.len = msg->routing_id.len;
  ret = (*nrppa_messages_callback[pdu.choice.initiatingMessage->procedureCode][pdu.present - 1])(&nrppa_msg_info, &pdu);

  if (msg->routing_id.buf) {
    free(msg->routing_id.buf);
  }

  if (msg->nrppa_pdu.buf) {
    free(msg->nrppa_pdu.buf);
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_NRPPA_NRPPA_PDU, &pdu);
  return ret;
}
