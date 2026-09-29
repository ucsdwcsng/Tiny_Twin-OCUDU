# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(test_gtp test_gtp.cpp)
target_link_libraries(test_gtp PRIVATE GTPV1U GTest::gtest minimal_lib)
target_include_directories(test_gtp PRIVATE ${CMAKE_SOURCE_DIR}/openair2/NRUP/lib)
# the following dependencies should ideally not exist, but GTP requires them
target_link_libraries(test_gtp PRIVATE ITTI asn1_lte_rrc_hdrs asn1_nr_rrc_hdrs)
add_dependencies(tests test_gtp)
add_test(NAME test_gtp COMMAND ./test_gtp)
