# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(nr-cuup-load-test nr-cuup-load-test.c)
add_dependencies(nr-cuup-load-test params_libconfig)
target_link_libraries(nr-cuup-load-test PRIVATE GTPV1U ${T_LIB} utils ITTI e1ap
  SCTP_CLIENT)
target_link_libraries(nr-cuup-load-test PRIVATE asn1_lte_rrc_hdrs asn1_nr_rrc_hdrs) # ITTI

if(ENABLE_TESTS)
  # we add also nr-cuup as it is required for the test
  add_dependencies(tests nr-cuup-load-test nr-cuup)
  add_test(NAME nr_cuup_functional_test
           COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/nr-cuup-functional-test.sh ${CMAKE_CURRENT_SOURCE_DIR}/load-test.conf
           WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  set_tests_properties(nr_cuup_functional_test PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}")
endif()
