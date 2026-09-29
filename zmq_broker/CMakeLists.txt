cmake_minimum_required(VERSION 3.16)
project(zmq_broker CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED IMPORTED_TARGET libzmq)

add_executable(zmq_broker zmq_broker.cpp)
target_compile_options(zmq_broker PRIVATE -Wall -Wextra -Werror)
target_link_libraries(zmq_broker PRIVATE PkgConfig::ZMQ)
install(TARGETS zmq_broker RUNTIME DESTINATION bin)
