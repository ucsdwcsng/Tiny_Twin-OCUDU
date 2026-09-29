# Copyright (c) 2023 Microsoft Corporation.
# SPDX-License-Identifier: MIT

Include(FetchContent)

FetchContent_Declare(
  elfio
  GIT_REPOSITORY https://github.com/serge1/ELFIO.git
  GIT_TAG        Release_3.10 # or a later release
)

FetchContent_MakeAvailable(elfio)

# Add the include directory for the library
include_directories(${elfio_SOURCE_DIR})

add_executable(btf_dump
  "btf_dump.cpp"
  "options.cpp"
  "options.h"
  )

target_link_libraries(btf_dump PRIVATE
            "libbtf")
