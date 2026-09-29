// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <cstdint>

namespace ocudu {
namespace fapi {

/// Precoding and beamforming PDU.
struct tx_precoding_and_beamforming_pdu {
  /// Physical resource groups information.
  struct prgs_info {
    uint16_t pm_index;
  };

  uint16_t prg_size;
  /// [Implementation-defined] Only a single PRG is used.
  prgs_info prg;
};

} // namespace fapi
} // namespace ocudu
