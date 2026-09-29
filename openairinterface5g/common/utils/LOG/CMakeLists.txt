# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library(log_headers INTERFACE)
target_include_directories(log_headers INTERFACE .)
target_link_libraries(log_headers INTERFACE T_headers)

set(log_sources log.c)
if (ENABLE_LTTNG)
  find_package(LTTngUST 2.3.8 EXACT REQUIRED)
  set(log_sources ${log_sources} lttng-tp.c)
else()
  message(STATUS "LTTNG support disabled")
endif()
add_library(LOG ${log_sources})
target_link_libraries(LOG PRIVATE CONFIG_LIB)
target_link_libraries(LOG PUBLIC log_headers)
if (ENABLE_LTTNG)
  target_link_libraries(LOG PUBLIC lttng-ust)
endif()
