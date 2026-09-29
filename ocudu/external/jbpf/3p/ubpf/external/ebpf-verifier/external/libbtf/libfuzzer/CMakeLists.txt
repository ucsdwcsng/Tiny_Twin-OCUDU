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

find_program(CLANG clang)
set(CMAKE_C_COMPILER ${CLANG})
set(CMAKE_CXX_COMPILER ${CLANG})

add_executable(libbtf_fuzzer libfuzz_harness.cpp)

file(COPY ${CMAKE_SOURCE_DIR}/external/ebpf-samples/build/ DESTINATION corpus)

target_compile_options(libbtf_fuzzer
            PRIVATE -g -O1 -fsanitize=fuzzer
            )

target_link_libraries(libbtf_fuzzer
            PRIVATE -fsanitize=fuzzer
            )

target_link_libraries(libbtf_fuzzer PRIVATE
            "libbtf")
