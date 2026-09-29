# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library(nfapi_nr_lib
        src/nfapi_nr_p5.c
        src/nfapi_nr_p7.c)
target_link_libraries(nfapi_nr_lib PUBLIC nfapi_common nr_fapi_p5 nr_fapi_p7)
target_link_libraries(nfapi_nr_lib PRIVATE z)

add_library(nfapi_lte_lib
        src/nfapi_p4.c
        src/nfapi_lte_p5.c
        src/nfapi_lte_p7.c)
target_link_libraries(nfapi_lte_lib PUBLIC nfapi_common)
target_link_libraries(nfapi_lte_lib PRIVATE z)

if(OAI_WLS)
    target_compile_definitions(nfapi_nr_lib PRIVATE ENABLE_WLS)
else()
    target_compile_definitions(nfapi_nr_lib PRIVATE ENABLE_SOCKET)
endif()
