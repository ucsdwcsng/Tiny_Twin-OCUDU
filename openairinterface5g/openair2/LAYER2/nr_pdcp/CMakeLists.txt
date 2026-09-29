# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library(nr_pdcp_snow3g OBJECT
            nr_pdcp_integrity_nia1.c
            nr_pdcp_security_nea1.c)

if (ENABLE_TESTS)
  add_subdirectory(tests)
endif ()
