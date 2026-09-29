// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"

namespace ocudu {

class pusch_processor_spy : public pusch_processor
{
public:
  void process(span<uint8_t>                    data,
               unique_rx_buffer                 buffer,
               pusch_processor_result_notifier& notifier,
               const resource_grid_reader&      grid,
               const pdu_t&                     pdu) override
  {
    processed_method_been_called = true;

    // Notify completion of PUSCH UCI.
    if ((pdu.uci.nof_harq_ack != 0) || (pdu.uci.nof_csi_part1 != 0)) {
      notifier.on_uci({});
    }

    // Notify completion of PUSCH data.
    notifier.on_sch({});
  }

  bool has_process_method_been_called() const { return processed_method_been_called; }

private:
  bool processed_method_been_called = false;
};

} // namespace ocudu
