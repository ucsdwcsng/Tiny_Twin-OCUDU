# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set_directory_properties(PROPERTIES LABELS "ofh;MVP-ARCH-INTF-4")

add_executable(ofh_integration_test ofh_integration_test.cpp)
target_link_libraries(ofh_integration_test
        ocudulog
        ocudu_ru_ofh
        ocudu_phy_support
        ocudu_channel_precoder
        ocudu_support)
add_test(ofh_integration_test ofh_integration_test -d \'{0,1}\')
set_tests_properties(ofh_integration_test PROPERTIES LABELS "tsan;NO_MEMCHECK")
