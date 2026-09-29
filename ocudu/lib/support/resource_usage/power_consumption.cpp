// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/resource_usage/power_consumption.h"
#include "perf_event_powercap_reader_impl.h"
#include "powercap_energy_reader_impl.h"
#include "rapl_msr_energy_reader_impl.h"

using namespace ocudu;
using namespace resource_usage_utils;

namespace {

/// Dummy implementation of energy consumption reader.
class dummy_energy_consumption_reader : public energy_consumption_reader
{
  energy_consumption read_consumed_energy() const override { return {}; }
};

} // namespace

std::unique_ptr<energy_consumption_reader>
resource_usage_utils::build_energy_consumption_reader(ocudulog::basic_logger& logger)
{
  // First try to build Powercap energy consumption reader via sysfs.
  if (auto reader = build_sysfs_powercap_reader(logger)) {
    return reader;
  }

  // Try perf_event_open() based RAPL reader (requires CAP_PERFMON, kernel >= 5.8).
  if (auto reader = build_perf_event_reader(logger)) {
    return reader;
  }

  // Fall back to RAPL MSR reader (requires CAP_SYS_RAWIO).
  if (auto reader = build_rapl_msr_reader(logger)) {
    return reader;
  }

  logger.info("Energy consumption utils are not available.");
  return std::make_unique<dummy_energy_consumption_reader>();
}

uint64_t resource_usage_utils::calculate_energy_diff(uint64_t current_uj, uint64_t previous_uj)
{
  int64_t diff = current_uj - previous_uj;
  if (diff < 0) {
    diff = static_cast<int64_t>(UINT64_MAX - previous_uj + current_uj);
  }
  return diff;
}
