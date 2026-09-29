// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/precoding/precoding_codebook_configuration.h"
#include "ocudu/adt/to_array.h"
#include "ocudu/ran/precoding/precoding_codebook_properties.h"
#include "ocudu/ran/precoding/precoding_codebook_type2_helpers.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/ocudu_assert.h"
#include "fmt/format.h"
#include <algorithm>

using namespace ocudu;

/// List of PMI codebook configurations indexed by \c precoding_codebook_identifier.
static constexpr auto codebook_configurations = to_array<pmi_codebook_config>(
    {pmi_codebook_one_port{},
     pmi_codebook_two_port{},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_one, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::two_two, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_one, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::three_two, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::six_one, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_two, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::eight_one, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_three, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::six_two, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::twelve_one, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::four_four, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::eight_two, pmi_codebook_typeI_mode::one},
     pmi_codebook_typeI_single_panel{pmi_codebook_single_panel_config::sixteen_one, pmi_codebook_typeI_mode::one}});
static_assert(codebook_configurations.size() == pmi_codebook_id::max() + 1,
              "The number of codebook configurations does not match the number of identifiers.");

static pmi_codebook_id to_id(std::monostate)
{
  return 0;
}

static pmi_codebook_id to_id(pmi_codebook_one_port)
{
  return 0;
}

static pmi_codebook_id to_id(pmi_codebook_two_port)
{
  return 1;
}

static pmi_codebook_id to_id(const pmi_codebook_typeI_single_panel& codebook)
{
  ocudu_assert(codebook.mode == pmi_codebook_typeI_mode::one, "Unsupported mode.");
  return 2 + static_cast<unsigned>(codebook.n1_n2);
}

static pmi_codebook_id to_id(const pmi_codebook_typeII&)
{
  report_error("The Type II codebook configuration does not have a codebook identifier.");
}

pmi_codebook_id ocudu::to_pmi_codebook_identifier(const pmi_codebook_config& codebook)
{
  return std::visit([](const auto& item) { return to_id(item); }, codebook);
}

const pmi_codebook_config& ocudu::to_pmi_codebook_config(pmi_codebook_id identifier)
{
  return codebook_configurations[identifier.value()];
}

std::string ocudu::to_string(const pmi_codebook_config& codebook)
{
  struct overloaded {
    std::string operator()(std::monostate) const { return "none"; }
    std::string operator()(pmi_codebook_one_port) const { return "one port"; }
    std::string operator()(pmi_codebook_two_port) const { return "two port"; }
    std::string operator()(const pmi_codebook_typeI_single_panel& config) const
    {
      const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(config.n1_n2);
      return fmt::format("Type I mode {} single-panel {}-port {}x{}",
                         static_cast<unsigned>(config.mode),
                         get_precoding_codebook_antenna_ports(config),
                         panel_info.n1,
                         panel_info.n2);
    }
    std::string operator()(const pmi_codebook_typeII& config) const
    {
      const pmi_codebook_single_panel_info& panel_info = get_single_panel_info(config.n1_n2);
      unsigned                              nof_ports  = get_precoding_codebook_antenna_ports(config);
      return fmt::format("Type II {}-port {}x{} {} beams {}-PSK sbAmp={}",
                         nof_ports,
                         panel_info.n1,
                         panel_info.n2,
                         config.nof_beams.value(),
                         static_cast<uint8_t>(config.phase_alphabet_size),
                         config.subband_amplitude);
    }
  };

  return std::visit(overloaded{}, codebook);
}

unsigned ocudu::get_precoding_codebook_antenna_ports(const pmi_codebook_config& pmi_codebook)
{
  struct overloaded {
    unsigned operator()(std::monostate) const { return 0; }
    unsigned operator()(pmi_codebook_one_port) const { return 1; }
    unsigned operator()(pmi_codebook_two_port) const { return 2; }
    unsigned operator()(const pmi_codebook_typeI_single_panel& codebook) const
    {
      pmi_codebook_single_panel_info panel_config = get_single_panel_info(codebook.n1_n2);
      return 2 * panel_config.n1 * panel_config.n2;
    }
    unsigned operator()(const pmi_codebook_typeII& codebook) const
    {
      pmi_codebook_single_panel_info panel_config = get_single_panel_info(codebook.n1_n2);
      return 2 * panel_config.n1 * panel_config.n2;
    }
  };

  return std::visit(overloaded{}, pmi_codebook);
}

unsigned ocudu::get_precoding_codebook_max_rank(const pmi_codebook_config& pmi_codebook)
{
  struct overloaded {
    unsigned operator()(std::monostate) const { return 0; }
    unsigned operator()(pmi_codebook_one_port) const { return 1; }
    unsigned operator()(pmi_codebook_two_port) const { return 2; }
    unsigned operator()(const pmi_codebook_typeI_single_panel& codebook) const
    {
      // The Type I single-panel codebook supports up to eight layers.
      pmi_codebook_single_panel_info panel_config = get_single_panel_info(codebook.n1_n2);
      return std::min(2 * panel_config.n1 * panel_config.n2, 8U);
    }
    unsigned operator()(const pmi_codebook_typeII&) const
    {
      // The UE shall not report RI greater than two.
      return max_nof_typeII_layers;
    }
  };

  return std::visit(overloaded{}, pmi_codebook);
}
