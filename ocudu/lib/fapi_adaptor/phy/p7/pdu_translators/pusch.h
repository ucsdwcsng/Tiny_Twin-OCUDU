// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/messages/ul_pusch_pdu.h"
#include "ocudu/phy/upper/uplink_pdu_slot_repository.h"

namespace ocudu {
namespace fapi_adaptor {

class uci_part2_correspondence_repository;

/// Helper function that converts a PUSCH FAPI PDU into a PUSCH uplink slot PDU.
void convert_pusch_fapi_to_phy(uplink_pdu_slot_repository::pusch_pdu& pdu,
                               const fapi::ul_pusch_pdu&              fapi_pdu,
                               slot_point                             slot,
                               uint16_t                               num_rx_ant,
                               uci_part2_correspondence_repository&   part2_repo);

} // namespace fapi_adaptor
} // namespace ocudu
