// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/precoding/precoding_weight_matrix.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief Precoding matrix repository.
///
/// The repository stores precoding matrices that are accessible using a precoding matrix index.
class precoding_matrix_repository
{
public:
  explicit precoding_matrix_repository(std::vector<precoding_weight_matrix> repo_) : repo(std::move(repo_))
  {
    ocudu_assert(!repo.empty(), "Empty container");
  }

  /// Iterators.
  std::vector<precoding_weight_matrix>::const_iterator begin() const { return repo.begin(); }
  std::vector<precoding_weight_matrix>::const_iterator end() const { return repo.end(); }

  /// \brief Returns the precoding matrix associated to the given index.
  ///
  /// Index value must be valid, i.e. a precoding configuration must exist in the repository for that index.
  const precoding_weight_matrix& get_precoding_matrix(unsigned index) const;

  /// Returns the number of precoding matrix stored in the repository.
  unsigned size() const { return repo.size(); }

private:
  std::vector<precoding_weight_matrix> repo;
};

} // namespace fapi_adaptor
} // namespace ocudu
