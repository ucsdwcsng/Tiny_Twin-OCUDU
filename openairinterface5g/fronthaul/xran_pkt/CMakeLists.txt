# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library(xran_pkt STATIC xran_pkt_api.c)
target_include_directories(xran_pkt PUBLIC ./)
target_compile_options(xran_pkt PRIVATE ${dpdk_CFLAGS})
target_link_libraries(xran_pkt PRIVATE ${dpdk_LIBRARIES})
target_include_directories(xran_pkt PUBLIC ${dpdk_INCLUDE_DIRS})

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()
