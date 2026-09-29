# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_subdirectory(MESSAGES)

# Library for RRC cell management functions
add_library(rrc_cell_management STATIC rrc_cell_management.c)
target_link_libraries(rrc_cell_management PUBLIC asn1_nr_rrc asn1_lte_rrc)
target_link_libraries(rrc_cell_management PRIVATE utils LOG asn1_nr_rrc_hdrs asn1_lte_rrc_hdrs)

if(ENABLE_TESTS)
  add_subdirectory(tests)
endif()
