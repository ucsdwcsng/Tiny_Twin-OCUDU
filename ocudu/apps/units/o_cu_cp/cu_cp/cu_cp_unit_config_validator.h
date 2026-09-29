// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_cp_unit_config.h"

namespace ocudu {

/// Validates the given CU-CP unit configuration. Returns true on success, false otherwise.
bool validate_cu_cp_unit_config(const cu_cp_unit_config& config);

} // namespace ocudu
