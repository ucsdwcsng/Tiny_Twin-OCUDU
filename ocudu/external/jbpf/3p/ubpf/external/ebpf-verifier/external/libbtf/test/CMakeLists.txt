# Copyright (c) 2023 Microsoft Corporation.
# SPDX-License-Identifier: MIT

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
  elfio
  GIT_REPOSITORY https://github.com/serge1/ELFIO.git
  GIT_TAG        Release_3.10 # or a later release
)

FetchContent_MakeAvailable(elfio)

add_executable(tests test.cpp)

# Add the include directory for the library
include_directories(${elfio_SOURCE_DIR})

target_link_libraries(tests PRIVATE
    Catch2::Catch2WithMain
    "libbtf")