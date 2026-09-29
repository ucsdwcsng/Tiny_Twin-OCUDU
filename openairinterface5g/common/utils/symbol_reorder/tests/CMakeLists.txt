# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(test_symbol_reorder test_symbol_reorder.cpp)
add_dependencies(tests test_symbol_reorder)
target_link_libraries(test_symbol_reorder PRIVATE symbol_reorder GTest::gtest)
add_test(NAME test_symbol_reorder COMMAND ./test_symbol_reorder)
