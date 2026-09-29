// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/rrc_nr/ul_ccch_msg_ies.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/rrc/rrc_types.h"

namespace ocudu {
namespace ocucp {

/// Converts a hex string (e.g. 01FA02) to a sec_as_key.
security::sec_key make_sec_key(std::string hex_str);

/// Converts a hex string (e.g. 01FA02) to a sec_128_as_key.
security::sec_128_key make_sec_128_key(std::string hex_str);

/// \brief Generates a dummy meas config
rrc_meas_cfg generate_dummy_meas_config();

/// \brief Constructs full RRC Reconfig request with radioBearerConfig, measConfig, masterCellGroup and NAS PDU
rrc_reconfiguration_procedure_request generate_rrc_reconfiguration_procedure_request();

/// \brief Generate RRC Container with invalid RRC Reestablishment Request.
byte_buffer generate_invalid_rrc_reestablishment_request_pdu(pci_t pci, rnti_t c_rnti);

/// \brief Generate RRC Container with valid RRC Reestablishment Request.
byte_buffer generate_valid_rrc_reestablishment_request_pdu(
    pci_t                       pci,
    rnti_t                      c_rnti,
    std::string                 short_mac_i = "0111011100001000",
    asn1::rrc_nr::reest_cause_e cause       = asn1::rrc_nr::reest_cause_opts::options::other_fail);

/// \brief Generate RRC Container with RRC Reestablishment Complete.
byte_buffer generate_rrc_reestablishment_complete_pdu();

/// \brief Generate RRC Container with Measurement Report
byte_buffer generate_measurement_report_pdu();

} // namespace ocucp
} // namespace ocudu
