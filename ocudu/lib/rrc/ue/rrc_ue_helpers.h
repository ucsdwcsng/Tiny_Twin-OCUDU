// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rrc_ue_logger.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/rrc/rrc_ue_capabilities.h"

namespace ocudu::ocucp {

// Logging.
typedef enum { Rx = 0, Tx } direction_t;

template <class T>
void log_rrc_message(rrc_ue_logger&    logger,
                     const direction_t dir,
                     byte_buffer_view  pdu,
                     const T&          msg,
                     srb_id_t          srb_id,
                     const char*       msg_type);

// UE Capabilities.

rrc_ue_capabilities_t get_capabilities(asn1::rrc_nr::ue_nr_cap_s& ue_capabilities, rrc_ue_logger& logger);

std::optional<rrc_ue_capabilities_t> get_capabilities(asn1::rrc_nr::ue_cap_rat_container_list_l& capabilities_list,
                                                      rrc_ue_logger&                             logger);

} // namespace ocudu::ocucp
