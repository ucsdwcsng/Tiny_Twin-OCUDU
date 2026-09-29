/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */
 
/*!
 * \brief NGAP gNb NAS procedure handler
 * @ingroup _ngap
 */

#ifndef NGAP_GNB_NAS_PROCEDURES_H_
#define NGAP_GNB_NAS_PROCEDURES_H_

#include <stdint.h>
#include "assertions.h"
#include "ngap_messages_types.h"
#include "ngap_msg_includes.h"

int ngap_gNB_handle_nas_downlink(sctp_assoc_t assoc_id, uint32_t stream, NGAP_NGAP_PDU_t *pdu);

int ngap_gNB_nas_uplink(instance_t instance, ngap_uplink_nas_t *ngap_uplink_nas_p);

int ngap_gNB_nas_non_delivery_ind(instance_t instance,
                                  ngap_nas_non_delivery_ind_t *ngap_nas_non_delivery_ind);

int ngap_gNB_handle_nas_first_req(
  instance_t instance, ngap_nas_first_req_t *ngap_nas_first_req_p);

int ngap_gNB_initial_ctxt_resp(
  instance_t instance, ngap_initial_context_setup_resp_t *initial_ctxt_resp_p);

int ngap_gNB_initial_ctxt_fail(instance_t instance, ngap_initial_context_setup_fail_t *initial_ctxt_fail);

int ngap_gNB_ue_capabilities(instance_t instance,
                             ngap_ue_cap_info_ind_t *ue_cap_info_ind_p);

#endif /* NGAP_GNB_NAS_PROCEDURES_H_ */
