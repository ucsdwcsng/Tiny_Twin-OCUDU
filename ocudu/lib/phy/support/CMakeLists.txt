# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_phy_support STATIC
        interpolator/interpolator_linear_impl.cpp
        time_alignment_estimator/time_alignment_estimator_dft_impl.cpp
        time_alignment_estimator/time_alignment_estimator_factories.cpp
        rb_allocation.cpp
        re_pattern.cpp
        resource_grid_impl.cpp
        resource_grid_mapper_impl.cpp
        resource_grid_pool_impl.cpp
        resource_grid_reader_impl.cpp
        resource_grid_writer_impl.cpp
        support_factories.cpp)
target_link_libraries(ocudu_phy_support ocuduvec ocudu_instrumentation ocudu_ran ocudu_support_math)

add_to_exported_libs(ocudu_phy_support)
