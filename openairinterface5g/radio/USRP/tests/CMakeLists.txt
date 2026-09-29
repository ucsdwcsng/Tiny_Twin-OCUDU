# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(test_usrp_converters test_usrp_converters.cpp ../usrp_converters.cpp)
target_link_libraries(test_usrp_converters PRIVATE GTest::gtest benchmark::benchmark ${UHD_LIBRARIES})
target_include_directories(test_usrp_converters PRIVATE ${UHD_INCLUDE_DIRS} .. ${CMAKE_SOURCE_DIR})
add_dependencies(tests test_usrp_converters)
add_test(NAME test_usrp_converters
  COMMAND ./test_usrp_converters)
