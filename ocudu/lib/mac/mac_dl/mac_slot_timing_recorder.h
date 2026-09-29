// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/du_types.h"
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace ocudu {

/// \brief Per-slot capture of the MAC slot processing timeline (Tiny-Twin instrumentation).
///
/// Enabled by setting the environment variable OCUDU_TTI_LOG_PATH. Samples are stored in a preallocated buffer from
/// the slot executor and written as CSV when the recorder is destroyed, so the gNB must shut down cleanly for the file
/// to be produced. Cell 0 writes to the given path, other cells to "<path>.cell<index>".
class mac_slot_timing_recorder
{
public:
  using clock = std::chrono::steady_clock;

  struct sample {
    /// Slot count, including hyper-SFN. A gap between consecutive rows means slot indications were skipped.
    uint64_t slot_count;
    /// Lower layers signalled the slot indication.
    clock::time_point enqueue_tp;
    /// MAC slot executor started handling the slot indication.
    clock::time_point start_tp;
    /// Scheduler returned the slot decision.
    clock::time_point sched_done_tp;
    /// All slot results were handed to the PHY.
    clock::time_point results_done_tp;
  };

  explicit mac_slot_timing_recorder(du_cell_index_t cell_index);
  ~mac_slot_timing_recorder();

  bool enabled() const { return not path.empty(); }

  void record(const sample& s) noexcept
  {
    if (samples.size() < max_samples) {
      samples.push_back(s);
    } else {
      ++nof_dropped;
    }
  }

private:
  /// About 16 minutes at 30 kHz SCS, ~80 MB.
  static constexpr std::size_t max_samples = 2'000'000;

  ocudulog::basic_logger& logger;
  std::string             path;
  std::vector<sample>     samples;
  uint64_t                nof_dropped = 0;
};

} // namespace ocudu
