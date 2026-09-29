# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library(tracer_utils OBJECT configuration.c database.c event.c handler.c utils.c)
target_link_libraries(tracer_utils PUBLIC m pthread)

add_executable(record record.c)
target_link_libraries(record PRIVATE tracer_utils)

add_executable(replay replay.c)
target_link_libraries(replay PRIVATE tracer_utils)

add_executable(extract_config extract_config.c)
target_link_libraries(extract_config PRIVATE tracer_utils)

add_executable(extract_input_subframe extract_input_subframe.c)
target_link_libraries(extract_input_subframe PRIVATE tracer_utils)

add_executable(extract_output_subframe extract_output_subframe.c)
target_link_libraries(extract_output_subframe PRIVATE tracer_utils)

add_executable(extract extract.c)
target_link_libraries(extract PRIVATE tracer_utils)

add_executable(macpdu2wireshark macpdu2wireshark.c)
target_link_libraries(macpdu2wireshark PRIVATE tracer_utils)

add_executable(csv csv.c)
target_link_libraries(csv PRIVATE tracer_utils)

add_executable(multi multi.c)
target_link_libraries(multi PRIVATE tracer_utils T)
target_include_directories(multi PRIVATE ..)

add_executable(textlog textlog.c)
target_link_libraries(textlog PRIVATE tracer_utils tracer_filter
                      tracer_logger tracer_view)

add_subdirectory(filter)
add_subdirectory(logger)
add_subdirectory(view)

# Option to enable record_db tracer tool with ClickHouse database support
add_boolean_option(T_RECORD_DB OFF "Compile record_db tracer tool with ClickHouse support" OFF)

if(T_RECORD_DB)
  # Save current BUILD_SHARED_LIBS setting
  set(SAVED_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

  # Configure options for clickhouse-cpp
  set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

  # Save current compiler flags and disable -Werror for zstd
  set(SAVED_CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

  # Check if system zstd library is available
  find_library(ZSTD_LIBRARY NAMES zstd)
  if(ZSTD_LIBRARY)
    # Use system zstd library to avoid building legacy zstd code with warnings
    set(WITH_SYSTEM_ZSTD ON CACHE BOOL "" FORCE)
  else()
    # Fall back to bundled zstd and suppress warnings from legacy code
    # https://github.com/ClickHouse/clickhouse-cpp/blob/master/contrib/zstd/zstd/legacy/zstd_v01.c#L1935
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-maybe-uninitialized -Wno-uninitialized")
  endif()

  CPMAddPackage(
    NAME clickhouse-cpp
    GITHUB_REPOSITORY ClickHouse/clickhouse-cpp
    GIT_TAG master
  )

  # Restore original compiler flags and BUILD_SHARED_LIBS
  set(CMAKE_C_FLAGS "${SAVED_CMAKE_C_FLAGS}")
  set(BUILD_SHARED_LIBS ${SAVED_BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)

  CPMAddPackage(
    NAME fmtlog
    GITHUB_REPOSITORY MengRao/fmtlog
    GIT_TAG main
    GIT_SHALLOW FALSE
  )

  add_executable(record_db record_db.cpp)

  target_include_directories(record_db PRIVATE ${fmtlog_SOURCE_DIR} ${fmtlog_SOURCE_DIR}/fmt/include)
  target_link_libraries(record_db PRIVATE tracer_utils clickhouse-cpp-lib fmtlog-shared fmt)

endif() # T_RECORD_DB

add_custom_target(T_tools)
add_dependencies(T_tools
                 record replay extract_config extract_input_subframe
                 extract_output_subframe extract macpdu2wireshark multi
                 textlog csv)

# Add record_db to T_tools if it was built
if(TARGET record_db)
  add_dependencies(T_tools record_db)
endif()

add_dependencies(nr-softmodem T_tools)
add_dependencies(nr-uesoftmodem T_tools)
add_dependencies(lte-softmodem T_tools)
add_dependencies(lte-uesoftmodem T_tools)

# Some T tracer tools need additional libraries (Xft, also PNG) that come with
# a long list of dependencies. We put it behind a separate option for
# GUI-related tools so they can be added on demand, if necessary.
add_boolean_option(T_TRACER_GUI OFF  "Compile T tracer GUI tools" OFF)
if(T_TRACER_GUI)
  add_library(tracer_events OBJECT event_selector.c)

  find_library(png png REQUIRED)
  add_executable(enb enb.c)
  target_link_libraries(enb PRIVATE tracer_utils tracer_filter tracer_gui
                        tracer_logger tracer_view tracer_events)
  target_link_libraries(enb PRIVATE png)

  add_executable(gnb gnb.c)
  target_link_libraries(gnb PRIVATE tracer_utils tracer_filter tracer_gui
                        tracer_logger tracer_view tracer_events)
  target_link_libraries(gnb PRIVATE png)

  add_executable(gnb_mac gnb_mac.c)
  target_link_libraries(gnb_mac PRIVATE tracer_utils tracer_filter tracer_gui
                        tracer_logger tracer_view tracer_events)

  add_executable(ue ue.c)
  target_link_libraries(ue PRIVATE tracer_utils tracer_view tracer_gui
                        tracer_logger tracer_filter tracer_events)
  target_link_libraries(ue PRIVATE png)

  add_executable(vcd vcd.c)
  target_link_libraries(vcd PRIVATE tracer_utils tracer_filter tracer_gui
                        tracer_logger tracer_view tracer_events)

  add_executable(to_vcd to_vcd.c)
  target_link_libraries(to_vcd PRIVATE tracer_utils tracer_filter tracer_gui
                        tracer_logger tracer_view tracer_events)

  add_subdirectory(gui)

  add_dependencies(T_tools enb gnb gnb_mac ue vcd to_vcd)
endif()
