// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/p7_slot_indication_notifier.h"
#include <chrono>

namespace ocudu {

/// Slot point extender adaptor. This object listens to FAPI SLOT.indication messages from the Split 6 O-DU high and
/// generates an extended slot point using the host time to derive the hyper-SFN and the given slot in the indication.
class slot_point_extender_adaptor : public fapi::p7_slot_indication_notifier
{
  const std::chrono::microseconds    slot_duration;
  fapi::p7_slot_indication_notifier& notifier;

public:
  slot_point_extender_adaptor(std::chrono::microseconds slot_duration_, fapi::p7_slot_indication_notifier& notifier_) :
    slot_duration(slot_duration_), notifier(notifier_)
  {
  }

  void on_slot_indication(const fapi::slot_indication& msg) override;
};

} // namespace ocudu
