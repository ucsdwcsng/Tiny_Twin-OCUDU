cmake_minimum_required(VERSION 3.16)

project(srsranVerifier)

######## srsran verifier library ########

set(SRSRAN_VERIFIER_LIB srsran_verifier)
set(SRSRAN_VERIFIER_SRC_DIR ${PROJECT_SOURCE_DIR})
set(SRSRAN_VERIFIER_SOURCES ${SRSRAN_VERIFIER_SRC_DIR}/srsran_verifier.cpp)
set(SRSRAN_VERIFIER_SPECS_HEADERS ${SRSRAN_VERIFIER_SRC_DIR}/specs)
set(SRSRAN_VERIFIER_HEADERS ${SRSRAN_VERIFIER_SRC_DIR} ${SRSRAN_VERIFIER_SPECS_HEADERS})

set(JBPF_EBPF_VERIFIER_HEADERS ${JBPF_PATH}/3p/ebpf-verifier/external ${JBPF_PATH}/3p/ebpf-verifier/src)
set(JBPF_VERIFIER_HEADERS ${JBPF_PATH}/src/verifier ${JBPF_EBPF_VERIFIER_HEADERS})

add_library(${SRSRAN_VERIFIER_LIB} ${SRSRAN_VERIFIER_SOURCES})
add_library(srsran::verifier_lib ALIAS ${SRSRAN_VERIFIER_LIB})
target_include_directories(${SRSRAN_VERIFIER_LIB} PUBLIC ${SRSRAN_VERIFIER_HEADERS} ${JBPF_VERIFIER_HEADERS} )

set_target_properties(${SRSRAN_VERIFIER_LIB}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY  "${OUTPUT_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY  "${OUTPUT_DIR}/lib"
)

add_custom_command(TARGET ${SRSRAN_VERIFIER_LIB} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/inc/
  COMMAND ${CMAKE_COMMAND} -E copy  ${SRSRAN_VERIFIER_SRC_DIR}/srsran_verifier.h* ${OUTPUT_DIR}/inc/  
)


######## srsran verifier cli ########

set(SRSRAN_VERIFIER_CLI srsran_verifier_cli)
set(SRSRAN_VERIFIER_CLI_SOURCES ${SRSRAN_VERIFIER_SRC_DIR}/srsran_verifier_cli.cpp)

add_executable(${SRSRAN_VERIFIER_CLI} ${SRSRAN_VERIFIER_CLI_SOURCES})
target_include_directories(${SRSRAN_VERIFIER_CLI} PUBLIC ${SRSRAN_VERIFIER_HEADERS} ${JBPF_EBPF_VERIFIER_HEADERS})
target_link_libraries(${SRSRAN_VERIFIER_CLI} jbpf::verifier_lib ${SRSRAN_VERIFIER_LIB} jbpf::verifier_lib)


set_target_properties(${SRSRAN_VERIFIER_CLI}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/bin"
)

######## Export header files for other projects ############
set(SRSRAN_VERIFIER_SPECS_HEADERS ${SRSRAN_VERIFIER_SPECS_HEADERS} PARENT_SCOPE)
