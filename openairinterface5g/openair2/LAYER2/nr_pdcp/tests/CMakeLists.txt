# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(snow3g_tests snow3g_tests.c)
target_link_libraries(snow3g_tests PRIVATE nr_pdcp_snow3g minimal_lib SECURITY)
add_test(NAME snow3g_tests
         COMMAND snow3g_tests)
add_dependencies(tests snow3g_tests)
