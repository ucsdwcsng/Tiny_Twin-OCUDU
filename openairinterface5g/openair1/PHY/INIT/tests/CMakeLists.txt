# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(test_nr_frame_params test_nr_frame_params.cpp)
target_link_libraries(test_nr_frame_params PRIVATE LOG minimal_lib PHY_NR -lm GTest::gtest)
add_dependencies(tests test_nr_frame_params)
add_test(NAME test_nr_frame_params COMMAND ./test_nr_frame_params)
