// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/messages/ul_pucch_pdu.h"
#include "ocudu/phy/upper/uplink_pdu_slot_repository.h"

namespace ocudu {
namespace fapi_adaptor {

/// Helper function that converts a PUCCH FAPI PDU into a PUCCH uplink slot PDU using the system frame number, slot and
/// number of reception antennas.
void convert_pucch_fapi_to_phy(uplink_pdu_slot_repository::pucch_pdu& pdu,
                               const fapi::ul_pucch_pdu&              fapi_pdu,
                               slot_point                             slot,
                               uint16_t                               num_rx_ant);

} // namespace fapi_adaptor
} // namespace ocudu
