// Copyright (c) Prevail Verifier contributors.
// SPDX-License-Identifier: MIT

#pragma once

#include "btf.h"

#include <vector>

namespace libbtf {
/**
 * @brief Serialize a vector of btf_kind into .BTF section format.
 *
 * @param[in] btf_kind The vector of btf_kind to serialize.
 * @return The serialized .BTF section.
 */
std::vector<std::byte> btf_write_types(const std::vector<btf_kind> &btf_kind);
} // namespace libbtf