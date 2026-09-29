/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief rrc GTPV1U procedures for eNB
 */

#ifndef RRC_ENB_GTPV1U_H_
#define RRC_ENB_GTPV1U_H_

#include "rrc_defs.h"

/*! \fn rrc_eNB_process_GTPV1U_CREATE_TUNNEL_RESP(const protocol_ctxt_t* const ctxt_pP, const gtpv1u_enb_create_tunnel_resp_t * const create_tunnel_resp_pP)
 *\brief Process GTPV1U_ENB_CREATE_TUNNEL_RESP message received from GTPV1U, retrieve the enb teid created.
 *\param ctxt_pP Running context
 *\param create_tunnel_resp_pP Message received by RRC.
 *\return 0 when successful, -1 if the UE index can not be retrieved. */
int
rrc_eNB_process_GTPV1U_CREATE_TUNNEL_RESP(
  const protocol_ctxt_t *const ctxt_pP,
  const gtpv1u_enb_create_tunnel_resp_t *const create_tunnel_resp_pP,
  uint8_t                         *inde_list
);

/*! \fn rrc_eNB_send_GTPV1U_ENB_DELETE_TUNNEL_REQ(module_id_t enb_mod_idP, const rrc_eNB_ue_context_t* const ue_context_pP)
 *\brief Send GTPV1U_ENB_DELETE_TUNNEL_REQ message to GTPV1U to destroy all UE-related tunnels.
 *\param module_id Instance ID of eNB.
 *\param ue_context_pP UE context in the eNB.
 */
void rrc_eNB_send_GTPV1U_ENB_DELETE_TUNNEL_REQ(
  module_id_t enb_mod_idP,
  rrc_eNB_ue_context_t* ue_context_pP
);

#endif /* RRC_ENB_GTPV1U_H_ */
