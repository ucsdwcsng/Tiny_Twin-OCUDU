# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(test_config_cmdline test_config_cmdline.cpp)
target_link_libraries(test_config_cmdline PRIVATE CONFIG_LIB GTest::gtest)
add_dependencies(test_config_cmdline params_libconfig)
add_dependencies(tests test_config_cmdline)

configure_file(test_config.conf test_config.conf COPYONLY)

add_test(NAME test_config_cmdline COMMAND ./test_config_cmdline)
set_tests_properties(test_config_cmdline PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}")
