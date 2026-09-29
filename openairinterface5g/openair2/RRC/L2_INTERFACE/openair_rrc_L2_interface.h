/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */

/*!
 * \brief layer 2 interface
 */
#ifndef __OPENAIR_RRC_L2_INTERFACE_H__
#define __OPENAIR_RRC_L2_INTERFACE_H__

#include "COMMON/mac_rrc_primitives.h"
#include "common/platform_types.h"

int8_t
mac_rrc_data_req(
  const module_id_t     module_idP,
  const int             CC_idP,
  const frame_t         frameP,
  const rb_id_t         srb_idP,
  const rnti_t          rntiP,
  const uint8_t         nb_tbP,
  uint8_t* const        buffer_pP,
  const uint8_t         mbsfn_sync_areaP
);

void mac_lite_sync_ind(
  const module_id_t module_idP,
  const uint8_t statusP);

void mac_rrc_meas_ind(
  const module_id_t,
  MAC_MEAS_REQ_ENTRY*const );

void
rlcrrc_data_ind(
  const protocol_ctxt_t* const ctxt_pP,
  const rb_id_t                rb_idP,
  const sdu_size_t             sdu_sizeP,
  const uint8_t * const        buffer_pP
);

uint8_t
pdcp_rrc_data_req(
  const protocol_ctxt_t* const ctxt_pP,
  const rb_id_t                rb_idP,
  const mui_t                  muiP,
  const confirm_t              confirmP,
  const sdu_size_t             sdu_buffer_sizeP,
  uint8_t* const               sdu_buffer_pP,
  const pdcp_transmission_mode_t modeP
);

void
pdcp_rrc_data_ind(
  const protocol_ctxt_t* const ctxt_pP,
  const rb_id_t                srb_idP,
  const sdu_size_t             sdu_sizeP,
  uint8_t              * const buffer_pP
);

void mac_out_of_sync_ind(
  const module_id_t module_idP,
  const frame_t frameP,
  const uint16_t CH_index);

char openair_rrc_eNB_init(
  const module_id_t module_idP);

int
mac_eNB_get_rrc_status(
  const module_id_t module_idP,
  const rnti_t      indexP
);
int
mac_UE_get_rrc_status(
  const module_id_t module_idP,
  const uint8_t     sig_indexP
);

void mac_in_sync_ind(
  const module_id_t module_idP,
  const frame_t frameP,
  const uint16_t eNB_indexP);

#endif
