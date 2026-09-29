

add_definitions(-DLIBUS_NO_SSL)

set(SOURCES
        eventing/epoll_kqueue.c
        bsd.c
        context.c
        loop.c
        socket.c
        udp.c)

add_library(uSockets STATIC ${SOURCES})
target_include_directories(uSockets PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(uSockets PRIVATE ${CMAKE_THREAD_LIBS_INIT})
add_to_exported_libs(uSockets)
