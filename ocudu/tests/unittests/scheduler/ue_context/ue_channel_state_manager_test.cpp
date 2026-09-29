// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/ue_context/ue_channel_state_manager.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

constexpr unsigned test_nof_rbs = 52;

ue_channel_state_manager make_channel_state_manager(unsigned nof_dl_ports)
{
  return ue_channel_state_manager(config_helpers::make_default_scheduler_expert_config().ue, nof_dl_ports);
}

} // namespace

// With a single antenna port no precoding is applied.
TEST(ue_channel_state_manager_test, single_port_uses_no_precoding)
{
  const ue_channel_state_manager csm = make_channel_state_manager(1);

  EXPECT_EQ(csm.get_nof_dl_layers(), 1);
  EXPECT_FALSE(csm.get_precoding(1, test_nof_rbs).has_value());
}

// With 2 antenna ports the initial precoding uses a two-antenna-port PMI for every supported number of layers.
TEST(ue_channel_state_manager_test, two_ports_use_two_antenna_port_pmi)
{
  const ue_channel_state_manager csm = make_channel_state_manager(2);

  EXPECT_EQ(csm.get_nof_dl_layers(), 1);
  for (unsigned nof_layers = 1; nof_layers <= 2; ++nof_layers) {
    const std::optional<pdsch_precoding_info> precoding = csm.get_precoding(nof_layers, test_nof_rbs);
    ASSERT_TRUE(precoding.has_value());
    ASSERT_FALSE(precoding->prg_infos.empty());
    EXPECT_TRUE(std::holds_alternative<pmi_two_antenna_port>(precoding->prg_infos[0]))
        << "unexpected PMI type for nof_layers=" << nof_layers;
  }
}

// With 4 antenna ports the initial precoding uses the Type-I single-panel two_one (N1=2, N2=1) codebook.
TEST(ue_channel_state_manager_test, four_ports_use_two_one_codebook)
{
  const ue_channel_state_manager csm = make_channel_state_manager(4);

  EXPECT_EQ(csm.get_nof_dl_layers(), 1);
  for (unsigned nof_layers = 1; nof_layers <= 4; ++nof_layers) {
    const std::optional<pdsch_precoding_info> precoding = csm.get_precoding(nof_layers, test_nof_rbs);
    ASSERT_TRUE(precoding.has_value());
    ASSERT_FALSE(precoding->prg_infos.empty());
    EXPECT_EQ(std::get<pmi_typeI_single_panel>(precoding->prg_infos[0]).panel_config.n1_n2,
              pmi_codebook_single_panel_config::two_one)
        << "unexpected codebook for nof_layers=" << nof_layers;
  }
}

// With 8 antenna ports the initial precoding uses the Type-I single-panel four_one (N1=4, N2=1) codebook for every
// supported number of layers, which is capped at a single codeword limit (MAX_NOF_LAYERS_PER_CODEWORD).
TEST(ue_channel_state_manager_test, eight_ports_use_four_one_codebook)
{
  const ue_channel_state_manager csm = make_channel_state_manager(8);

  EXPECT_EQ(csm.get_nof_dl_layers(), 1);
  for (unsigned nof_layers = 1; nof_layers <= pdsch_constants::MAX_NOF_LAYERS_PER_CODEWORD; ++nof_layers) {
    const std::optional<pdsch_precoding_info> precoding = csm.get_precoding(nof_layers, test_nof_rbs);
    ASSERT_TRUE(precoding.has_value());
    ASSERT_FALSE(precoding->prg_infos.empty());
    EXPECT_EQ(std::get<pmi_typeI_single_panel>(precoding->prg_infos[0]).panel_config.n1_n2,
              pmi_codebook_single_panel_config::four_one)
        << "unexpected codebook for nof_layers=" << nof_layers;
  }
}

// An RI above the single-codeword layer limit is clamped to MAX_NOF_LAYERS_PER_CODEWORD.
TEST(ue_channel_state_manager_test, ri_above_single_codeword_limit_is_limited)
{
  ue_channel_state_manager csm = make_channel_state_manager(8);

  csi_report_data report;
  report.ri = csi_report_data::ri_type{6};
  report.pmi =
      pmi_typeI_single_panel{.panel_config = {pmi_codebook_single_panel_config::four_one, pmi_codebook_typeI_mode::one},
                             .i_1_1        = 0,
                             .i_1_2        = std::nullopt,
                             .i_1_3        = std::nullopt,
                             .i_2          = 0};

  EXPECT_TRUE(csm.handle_csi_report(report));
  EXPECT_EQ(csm.get_nof_dl_layers(), pdsch_constants::MAX_NOF_LAYERS_PER_CODEWORD);
}

// An RI within the single-codeword layer limit is used verbatim.
TEST(ue_channel_state_manager_test, ri_within_limit_is_used)
{
  ue_channel_state_manager csm = make_channel_state_manager(8);

  csi_report_data report;
  report.ri = csi_report_data::ri_type{3};

  EXPECT_TRUE(csm.handle_csi_report(report));
  EXPECT_EQ(csm.get_nof_dl_layers(), 3);
}

// An RI that exceeds the number of DL ports is invalid and the report is rejected.
TEST(ue_channel_state_manager_test, ri_above_nof_ports_is_rejected)
{
  ue_channel_state_manager csm = make_channel_state_manager(4);

  csi_report_data report;
  report.ri = csi_report_data::ri_type{5};

  EXPECT_FALSE(csm.handle_csi_report(report));
}
