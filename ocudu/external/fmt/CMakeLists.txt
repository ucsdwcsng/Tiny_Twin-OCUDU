# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        src/format.cc
        src/os.cc)

add_library(fmt STATIC ${SOURCES})
target_link_libraries(fmt ${CMAKE_THREAD_LIBS_INIT})

add_to_exported_libs(fmt)

set(SBOM_FMT_VERSION "11.1.2" CACHE STRING "SBOM version for FMT")
