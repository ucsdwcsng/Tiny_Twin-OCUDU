# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_subdirectory(lib)
add_subdirectory(MESSAGES)

add_library(xnap xnap_common.c)
target_link_libraries(xnap PUBLIC asn1_xnap xnap_lib)

if(ENABLE_TESTS)
  add_subdirectory(tests)
endif()



