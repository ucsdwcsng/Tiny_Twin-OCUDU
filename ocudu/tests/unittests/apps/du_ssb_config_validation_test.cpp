// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "apps/du/du_appconfig.h"
#include "apps/du/du_appconfig_cli11_schema.h"
#include "apps/units/flexible_o_du/flexible_o_du_application_unit.h"
#include "yaml_roundtrip_test_helpers.h"
#include "ocudu/ran/beamforming/beam_identifier.h"
#include "ocudu/support/config_parsers.h"
#include "CLI/CLI11.hpp"

#ifndef CONFIGS_DIR
#error "CONFIGS_DIR must be defined"
#endif

using namespace ocudu;
using namespace ocudu::yaml_roundtrip_test;

namespace {

/// Example configuration on band n78 with 30kHz SSB SCS, which gives L_max 8.
const std::string BASE_CONFIG = std::string(CONFIGS_DIR) + "/du_rf_b200_tdd_n78_20mhz.yml";

/// L_max of the base configuration.
constexpr unsigned BASE_CONFIG_L_MAX = 8;

class du_ssb_config_validation_test : public ::testing::Test
{
protected:
  /// \brief Loads the base configuration with its SSB beams replaced by the given ones and validates it.
  /// \return True if the configuration is valid, false otherwise.
  /// \remark Throws \c CLI::ValidationError if a value is rejected while parsing, before validation is reached.
  static bool validate_with_ssb_beams(const std::vector<std::pair<unsigned, unsigned>>& ssb_index_to_beam_id)
  {
    YAML::Node node = YAML::Load(read_file(BASE_CONFIG));

    YAML::Node beams;
    for (const auto& [ssb_index, beam_id] : ssb_index_to_beam_id) {
      YAML::Node beam_node;
      beam_node["ssb_index"] = ssb_index;
      beam_node["beam_id"]   = beam_id;
      beams.push_back(beam_node);
    }
    node["cell_cfg"]["ssb"]["beams"] = beams;

    temp_yaml_file tmp(YAML::Dump(node));

    CLI::App app("du ssb-config-validation-test");
    app.config_formatter(create_yaml_config_parser());
    app.allow_config_extras(CLI::config_extras_mode::error);
    std::string cfg_path;
    app.set_config("-c,", cfg_path, "Read config from file", false);

    du_appconfig du_cfg;
    configure_cli11_with_du_appconfig_schema(app, du_cfg);

    auto o_du = create_flexible_o_du_application_unit("du");
    o_du->on_parsing_configuration_registration(app);
    app.callback([&]() { o_du->on_configuration_parameters_autoderivation(app); });

    std::vector<const char*> argv = {"du", "-c", tmp.path().c_str()};
    app.parse(static_cast<int>(argv.size()), argv.data());

    return o_du->on_configuration_validation();
  }
};

} // namespace

TEST_F(du_ssb_config_validation_test, single_ssb_candidate_is_valid)
{
  ASSERT_TRUE(validate_with_ssb_beams({{0, 0}}));
}

TEST_F(du_ssb_config_validation_test, multiple_ssb_candidates_are_valid)
{
  ASSERT_TRUE(validate_with_ssb_beams({{0, 0}, {3, 5}, {7, 1}}));
}

TEST_F(du_ssb_config_validation_test, ssb_candidates_without_index_zero_are_valid)
{
  ASSERT_TRUE(validate_with_ssb_beams({{1, 0}, {2, 1}}));
}

TEST_F(du_ssb_config_validation_test, when_ssb_index_is_above_l_max_then_config_is_invalid)
{
  ASSERT_FALSE(validate_with_ssb_beams({{BASE_CONFIG_L_MAX, 0}}));
}

TEST_F(du_ssb_config_validation_test, when_ssb_index_is_repeated_then_config_is_invalid)
{
  ASSERT_FALSE(validate_with_ssb_beams({{1, 0}, {1, 3}}));
}

TEST_F(du_ssb_config_validation_test, when_beam_id_is_out_of_range_then_parsing_fails)
{
  // The beam ID range is enforced while parsing, so it never reaches the configuration validator.
  ASSERT_THROW(validate_with_ssb_beams({{0, max_nof_beams}}), CLI::ValidationError);
}
