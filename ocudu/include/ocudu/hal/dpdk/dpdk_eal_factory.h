// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/hal/dpdk/dpdk_eal.h"
#include <memory>

namespace ocudu {
namespace dpdk {

/// Returns a dpdk_eal instance on success, otherwise returns nullptr.
std::unique_ptr<dpdk_eal> create_dpdk_eal(const std::string& args, ocudulog::basic_logger& logger);

} // namespace dpdk
} // namespace ocudu
