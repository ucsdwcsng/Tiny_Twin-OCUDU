# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library(nrup_lib OBJECT nrup_dl_user_data.c nrup_dl_data_delivery_status.c)
target_link_libraries(nrup_lib PRIVATE ds log_headers)
target_include_directories(nrup_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(nrup_lib PRIVATE ${CMAKE_SOURCE_DIR})
