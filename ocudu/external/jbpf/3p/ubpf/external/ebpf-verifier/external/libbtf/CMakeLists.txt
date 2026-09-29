# Copyright (c) 2023 Microsoft Corporation.
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.16)
project("btf")

if (BTF_INSTALL_GIT_HOOKS AND EXISTS "${PROJECT_SOURCE_DIR}/.git/hooks")
    # Install Git pre-commit hook
    file(COPY scripts/pre-commit scripts/commit-msg
        DESTINATION "${PROJECT_SOURCE_DIR}/.git/hooks")
endif()

include("cmake/platform.cmake")
include("cmake/options.cmake")
include("cmake/version.cmake")

add_subdirectory("libbtf")

if(BTF_ENABLE_TESTS)
# Suppress C4702 for MSVC
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4702")
endif()

  add_subdirectory("test")
endif()

if(BTF_DUMP)
  add_subdirectory("dump")
endif()

if (BTF_ENABLE_FUZZING)
  add_subdirectory("libfuzzer")
endif()
