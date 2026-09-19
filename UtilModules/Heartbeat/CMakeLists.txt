# Heartbeat

## Build libheartbeat.so
aux_source_directory    (${CMAKE_CURRENT_SOURCE_DIR} SRC_HEARTBEAT_PATH)

add_library             (LibHeartbeat SHARED ${SRC_HEARTBEAT_PATH})
set_target_properties   (LibHeartbeat PROPERTIES OUTPUT_NAME "heartbeat")
set_target_properties   (LibHeartbeat PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries   (LibHeartbeat stdc++ rt dl pthread)
