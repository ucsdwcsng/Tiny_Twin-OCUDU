// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_slot_timing_recorder.h"
#include <cstdlib>
#include <fstream>

using namespace ocudu;

static int64_t to_ns(mac_slot_timing_recorder::clock::time_point tp)
{
  return std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
}

mac_slot_timing_recorder::mac_slot_timing_recorder(du_cell_index_t cell_index) :
  logger(ocudulog::fetch_basic_logger("MAC"))
{
  const char* configured_path = std::getenv("OCUDU_TTI_LOG_PATH");
  if (configured_path == nullptr or configured_path[0] == '\0') {
    return;
  }
  path = configured_path;
  if (cell_index != to_du_cell_index(0)) {
    path += fmt::format(".cell{}", fmt::underlying(cell_index));
  }
  samples.reserve(max_samples);
  logger.info("Slot timing capture enabled: path={} capacity={}", path, max_samples);
}

mac_slot_timing_recorder::~mac_slot_timing_recorder()
{
  if (not enabled()) {
    return;
  }

  std::ofstream output(path, std::ios::out | std::ios::trunc);
  if (not output) {
    logger.error("Failed to open slot timing file {}", path);
    return;
  }

  output << "slot_count,enqueue_ns,start_ns,sched_done_ns,results_done_ns\n";
  for (const sample& s : samples) {
    output << s.slot_count << ',' << to_ns(s.enqueue_tp) << ',' << to_ns(s.start_tp) << ','
           << to_ns(s.sched_done_tp) << ',' << to_ns(s.results_done_tp) << '\n';
  }
  output.close();

  if (not output) {
    logger.error("Failed while writing slot timing file {}", path);
    return;
  }
  logger.info("Wrote {} slot timing samples to {} ({} dropped, buffer full)", samples.size(), path, nof_dropped);
}
