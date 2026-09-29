cmake_minimum_required(VERSION 3.16)

# Project name
project(srsranReverseProxyProject LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_FLAGS "-std=c++17")

# Binary name
set(BINARY_NAME srsran_reverse_proxy)

# check that required jbpf libraries are present
#set(JBPF_VERIFIER_LIB ${JBPF_LIB_DIR}/libjbpf_verifier.a)
#message(STATUS "JBPF_VERIFIER_LIB: ${JBPF_VERIFIER_LIB}")
#if(NOT EXISTS "${JBPF_VERIFIER_LIB}")
#    message(FATAL_ERROR "Required file not found at ${JBPF_VERIFIER_LIB}. Please ensure the file exists before running CMake.")
#endif()


set(JBPF_REVERSE_PROXY_PATH $ENV{JBPF_PATH}/examples/reverse_proxy)
set(JBPF_REVERSE_PROXY_LIB ${JBPF_REVERSE_PROXY_PATH}/libreverse_proxy.a)
#message(STATUS "JBPF_REVERSE_PROXY_LIB: ${JBPF_REVERSE_PROXY_LIB}")
#if(NOT EXISTS "${JBPF_REVERSE_PROXY_LIB}")
#    message(FATAL_ERROR "Required file not found at ${JBPF_REVERSE_PROXY_LIB}. Please ensure the file exists before running CMake.")
#endif()


# Include directories
include_directories(
    $ENV{JBPF_OUT_DIR}/inc
    $ENV{JBPF_PATH}/examples/reverse_proxy
)

# Linker flags
set(LINK_LIBS
    srsran::verifier_lib
    jbpf::core_lib 
    jbpf::lcm_ipc_lib
    jbpf::verifier_lib
    jbpf::reverse_proxy_lib

    boost_program_options
    ck
    mimalloc
    pthread
    dl
    rt
)


link_directories($ENV{JBPF_OUT_DIR}/lib ${JBPF_REVERSE_PROXY_PATH})

# Add executable for main.cpp and link with the static library
add_executable(${BINARY_NAME} main.cpp)

# store executable in the current directory
set_target_properties(${BINARY_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/bin"
)

# Link the static library and additional libraries with the executable
target_link_libraries(${BINARY_NAME} ${LINK_LIBS} ${LINK_LIBS} ${LINK_FLAGS})

