// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/s_nssai.h"
#include "ocudu/ran/tac.h"
#include <vector>

namespace ocudu::ocucp {

struct plmn_item {
  plmn_identity          plmn_id;
  std::vector<s_nssai_t> slice_support_list;
};

struct supported_tracking_area {
  tac_t                  tac;
  std::vector<plmn_item> plmn_list;
};

} // namespace ocudu::ocucp
