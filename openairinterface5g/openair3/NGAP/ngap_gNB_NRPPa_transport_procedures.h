/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

#ifndef NGAP_GNB_NRPPA_PROCEDURES_H_
#define NGAP_GNB_NRPPA_PROCEDURES_H_

// UPLINK UE ASSOCIATED NRPPA TRANSPORT (9.2.9.2 of TS 38.413 Version 16.0.0)
int ngap_gNB_uplink_ue_associated_nrppa_transport(instance_t instance, const ngap_uplink_ue_associated_nrppa_t *msg);

// UPLINK NON UE ASSOCIATED NRPPA TRANSPORT (9.2.9.4 of TS 38.413 Version 16.0.0)
int ngap_gNB_uplink_non_ue_associated_nrppa_transport(instance_t instance, const ngap_uplink_non_ue_associated_nrppa_t *msg);

// DOWNLINK UE ASSOCIATED NRPPA TRANSPORT (9.2.9.1 of TS 38.413 Version 16.0.0)
int ngap_gNB_handle_downlink_ue_associated_nrppa_transport(sctp_assoc_t assoc_id, uint32_t stream, NGAP_NGAP_PDU_t *pdu);

// DOWNLINK UE ASSOCIATED NRPPA TRANSPORT (9.2.9.3 of TS 38.413 Version 16.0.0)
int ngap_gNB_handle_downlink_non_ue_associated_nrppa_transport(sctp_assoc_t assoc_id, uint32_t stream, NGAP_NGAP_PDU_t *pdu);

#endif /* NGAP_GNB_NRPPA_PROCEDURES_H_ */
