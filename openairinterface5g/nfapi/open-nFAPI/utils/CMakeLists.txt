# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(nfapi_hex_parser nfapi_hex_parser.c)
target_link_libraries(nfapi_hex_parser PUBLIC nr_fapi_p5 nr_fapi_p7)
# This makes it so nfapi_hex_parser is always compiled whenever nfapi_nr_lib is compiled
add_dependencies(nfapi_nr_lib nfapi_hex_parser )
