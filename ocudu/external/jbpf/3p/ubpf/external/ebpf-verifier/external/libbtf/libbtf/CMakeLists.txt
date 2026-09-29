# Copyright (c) 2023 Microsoft Corporation.
# SPDX-License-Identifier: MIT


add_library("libbtf" STATIC
    "btf.h"
    "btf_c_type.h"
    "btf_json.cpp"
    "btf_json.h"
    "btf_map.cpp"
    "btf_map.h"
    "btf_parse.cpp"
    "btf_parse.h"
    "btf_type_data.cpp"
    "btf_type_data.h"
    "btf_write.cpp"
    "btf_write.h"
)

target_include_directories("libbtf" INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
