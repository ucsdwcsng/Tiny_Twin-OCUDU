// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "adapters/f1u_adapters.h"
#include "adapters/gw_adapters.h"
#include "adapters/pdcp_adapters.h"
#include "qos_flow_context.h"
#include "ocudu/f1u/cu_up/f1u_config.h"
#include "ocudu/pdcp/pdcp_entity.h"
#include "ocudu/ran/rb_id.h"
#include <map>

namespace ocudu {
namespace ocuup {

/// \brief DRB context with map to all QoS flows.
struct drb_context {
  drb_context(const drb_id_t& drb_id_) : drb_id(drb_id_) {}
  ~drb_context() { stop(); }

  void stop()
  {
    if (!stopped) {
      if (pdcp) {
        pdcp->stop();
      }
      if (f1u) {
        f1u->stop();
      }
      if (f1u_gw_bearer) {
        f1u_gw_bearer->stop();
      }
      stopped = true;
    }
  }

  bool stopped = false;

  drb_id_t    drb_id;
  gtpu_teid_t f1u_ul_teid;
  f1u_config  f1u_cfg;

  std::unique_ptr<f1u_cu_up_gateway_bearer> f1u_gw_bearer;
  std::unique_ptr<f1u_bearer>               f1u;
  std::unique_ptr<pdcp_entity>              pdcp;

  // Adapter PDCP->SDAP
  // FIXME: Currently, we assume only one DRB per PDU session and only one QoS flow per DRB.
  pdcp_sdap_adapter pdcp_to_sdap_adapter;
  pdcp_f1u_adapter  pdcp_to_f1u_adapter;
  f1u_pdcp_adapter  f1u_to_pdcp_adapter;

  // Adapter PDCP->CU-UP Manager.
  // Used for control events.
  pdcp_rx_cu_up_mngr_adapter pdcp_rx_to_cu_up_mngr_adapter;
  pdcp_tx_cu_up_mngr_adapter pdcp_tx_to_cu_up_mngr_adapter;

  // Adapter F1-U gateway -> NR-U
  f1u_gateway_rx_nru_adapter f1u_gateway_rx_to_nru_adapter;

  uint8_t cell_group_id; /// This can/should be a list of cell groups.

  std::map<qos_flow_id_t, std::unique_ptr<qos_flow_context>> qos_flows; // key is qos_flow_id
};

} // namespace ocuup
} // namespace ocudu
