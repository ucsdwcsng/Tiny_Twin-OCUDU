// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace ofh {

struct tx_data_flow_perf_metrics;

/// Open Fronthaul transmitter data flow performance metrics collector interface.
class data_flow_message_encoding_metrics_collector
{
public:
  virtual ~data_flow_message_encoding_metrics_collector() = default;

  /// Collect the performance metrics of a data flow.
  virtual void collect_metrics(tx_data_flow_perf_metrics& metric) = 0;
};

} // namespace ofh
} // namespace ocudu
