// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <string>

namespace ocudu {

/// HAL configuration of the application.
struct hal_appconfig {
  /// EAL configuration arguments.
  std::string eal_args;
};

} // namespace ocudu
