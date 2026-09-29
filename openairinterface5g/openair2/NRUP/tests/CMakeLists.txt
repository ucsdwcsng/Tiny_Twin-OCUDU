# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(nrup_lib_test nrup_lib_test.c)
add_dependencies(tests nrup_lib_test)
add_test(NAME nrup_lib_test COMMAND nrup_lib_test)
target_link_libraries(nrup_lib_test PRIVATE nrup_lib ds LOG SIMU CONFIG_LIB m)
target_include_directories(nrup_lib_test PRIVATE
                           ${CMAKE_SOURCE_DIR}
                           ${CMAKE_SOURCE_DIR}/openair2/NRUP
)
