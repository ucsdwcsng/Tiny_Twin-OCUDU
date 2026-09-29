// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi_adaptor/precoding_matrix_repository.h"
#include <memory>

namespace ocudu {
namespace fapi_adaptor {

/// Precoding matrix repository builder.
class precoding_matrix_repository_builder
{
public:
  explicit precoding_matrix_repository_builder(unsigned size) { repository.reserve(size); }

  /// Adds the given precoding configuration to the repository with the given index.
  void add(unsigned index, const precoding_weight_matrix& precoding)
  {
    if (index >= repository.size()) {
      repository.resize(index + 1U);
    }

    repository[index] = precoding;
  }

  /// Builds and returns a precoding matrix repository.
  std::unique_ptr<precoding_matrix_repository> build()
  {
    return std::make_unique<precoding_matrix_repository>(std::move(repository));
  }

private:
  std::vector<precoding_weight_matrix> repository;
};

} // namespace fapi_adaptor
} // namespace ocudu
