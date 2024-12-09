## Build epollevent.so
aux_source_directory    (${CMAKE_CURRENT_SOURCE_DIR} SRC_EPOLL_EVENT_PATH)

add_library             (LibEpollEvent SHARED ${SRC_EPOLL_EVENT_PATH})
set_target_properties   (LibEpollEvent PROPERTIES OUTPUT_NAME "epollevent")
set_target_properties   (LibEpollEvent PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries   (LibEpollEvent stdc++ rt dl)
