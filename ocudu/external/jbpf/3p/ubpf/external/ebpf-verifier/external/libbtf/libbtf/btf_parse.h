// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT

#pragma once

#include "btf.h"

#include <functional>

namespace libbtf {
/**
 * @brief Visitor function invoked for each btf_type record.
 *
 */
using btf_type_visitor = std::function<void(
    btf_type_id, const std::optional<std::string> &, const btf_kind &)>;

/**
 * @brief Visitor function invoked for each btf_line_info record.
 *
 */
using btf_line_info_visitor =
    std::function<void(const std::string &section, uint32_t instruction_offset,
                       const std::string &file_name, const std::string &source,
                       uint32_t line_number, uint32_t column_number)>;

/**
 * @brief Parse a .BTF and .BTF.ext section from an ELF file invoke visitor for
 * each btf_line_info record.
 *
 * @param[in] btf The .BTF section (containing type info and strings).
 * @param[in] btf_ext The .BTF.ext section (containing function info and
 * line info).
 * @param[in] visitor Function to invoke on each btf_line_info record.
 */
void btf_parse_line_information(const std::vector<std::byte> &btf,
                                const std::vector<std::byte> &btf_ext,
                                btf_line_info_visitor visitor);

/**
 * @brief Parse a .BTF section from an ELF file and invoke visitor for each
 * btf_type record.
 *
 * @param[in] btf The .BTF section (containing type info and strings).
 * @param[in] visitor Function to invoke on each btf_type record.
 */
void btf_parse_types(const std::vector<std::byte> &btf,
                     btf_type_visitor visitor);
} // namespace libbtf