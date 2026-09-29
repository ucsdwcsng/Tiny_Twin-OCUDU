cmake_minimum_required(VERSION 3.16)

project(srsranJbpf)

set(JBPF_PATH $ENV{JBPF_PATH})
set(JBPF_LIB_DIR $ENV{JBPF_OUT_DIR}/lib)

set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out)

add_subdirectory(verifier)
add_subdirectory(reverse_proxy)


add_custom_target(copy-srsran-jbpf-headers ALL)
add_custom_command(TARGET copy-srsran-jbpf-headers POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/inc/
  COMMAND ${CMAKE_COMMAND} -E copy  ${JBPF_SRSRAN_HEADER_FILES}/* ${OUTPUT_DIR}/inc/
)


######## Export header files for other projects ############
set(SRSRAN_VERIFIER_SPECS_HEADERS ${SRSRAN_VERIFIER_SPECS_HEADERS} PARENT_SCOPE)

