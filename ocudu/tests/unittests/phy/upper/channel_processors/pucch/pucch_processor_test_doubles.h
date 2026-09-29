// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pucch/pucch_processor.h"

namespace ocudu {

class pucch_processor_dummy : public pucch_processor
{
public:
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override
  {
    return {};
  }

  pucch_format1_map<pucch_processor_result> process(const resource_grid_reader&        grid,
                                                    const format1_batch_configuration& config) override
  {
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override
  {
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    return {};
  }
};

} // namespace ocudu
