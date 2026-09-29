// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT

#pragma once
#include "btf.h"

#include "btf_type_data.h"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace libbtf {
struct btf_map_definition {
  std::string name;
  btf_type_id type_id;
  uint32_t map_type;
  uint32_t key_size;
  uint32_t value_size;
  uint32_t max_entries;
  btf_type_id inner_map_type_id;
};

/**
 * @brief Extract BTF map definitions from a BTF_KIND_DATA_SECTION section with
 * name ".maps".
 *
 * @param[in] btf BTF data.
 * @return A vector of BTF map definitions.
 */
std::vector<btf_map_definition>
parse_btf_map_section(const btf_type_data &btf_data);

/**
 * @brief Add a BTF_KIND_DATA_SECTION section with name ".maps" to a collection
 * of BTF data.
 *
 * @param[in] map_definitions
 * @param[in,out] btf_data
 */
void build_btf_map_section(
    const std::vector<btf_map_definition> &map_definitions,
    btf_type_data &btf_data);
} // namespace libbtf