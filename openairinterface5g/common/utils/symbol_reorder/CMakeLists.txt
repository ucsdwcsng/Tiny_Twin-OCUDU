# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library(symbol_reorder symbol_reorder.cpp)
target_include_directories(symbol_reorder PUBLIC .)

if (ENABLE_TESTS)
  add_subdirectory(tests)
endif()
