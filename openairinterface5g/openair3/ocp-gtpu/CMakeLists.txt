# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library (GTPV1U gtp_itf.cpp gtpu_extensions.c)
target_link_libraries(GTPV1U PRIVATE nrup_lib)
target_link_libraries(GTPV1U PRIVATE asn1_lte_rrc_hdrs asn1_nr_rrc_hdrs) # for ITTI
target_link_libraries(GTPV1U PRIVATE SIMU ds)
target_include_directories(GTPV1U PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(ENABLE_TESTS)
  add_subdirectory(tests)
endif()
