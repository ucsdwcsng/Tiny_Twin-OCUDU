// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT

#pragma once

#include "btf.h"

#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <string>

namespace libbtf {
/**
 * @brief Given a map of btf_type_id to btf_kind, print the types as JSON to
 * the given output stream. The JSON is not pretty printed. This is useful for
 * debugging and testing.
 *
 * @param[in] id_to_kind A map of btf_type_id to btf_kind.
 * @param[in,out] out The output stream to write the JSON to.
 */
void btf_type_to_json(
    const std::map<btf_type_id, btf_kind> &id_to_kind, std::ostream &out,
    std::optional<std::function<bool(btf_type_id)>> filter = std::nullopt);

/**
 * @brief Helper function to insert line breaks and indentation into a JSON
 * string to make it more human readable.
 *
 * @param[in] input JSON string to pretty print.
 * @return The pretty printed JSON string.
 */
std::string pretty_print_json(const std::string &input);
} // namespace libbtf