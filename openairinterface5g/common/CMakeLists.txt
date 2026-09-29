# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_subdirectory(utils)
add_subdirectory(config/yaml)
if (ENABLE_TESTS)
  add_subdirectory(config/tests)
endif()

configure_file(cmake_defs.h.in cmake_defs.h @ONLY)
configure_file(oai_version.h.in oai_version.h @ONLY)

add_library(instrumentation INTERFACE)
target_include_directories(instrumentation INTERFACE .)
if (TRACY_ENABLE)
  target_link_libraries(instrumentation INTERFACE Tracy::TracyClient)
endif()
