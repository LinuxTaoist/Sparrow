## Build codecx.so
aux_source_directory    (${CMAKE_CURRENT_SOURCE_DIR} SRC_CODEX_PATH)

add_library             (LibCodecX SHARED ${SRC_CODEX_PATH})
set_target_properties   (LibCodecX PROPERTIES OUTPUT_NAME "codex")
set_target_properties   (LibCodecX PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries   (LibCodecX stdc++ rt dl)
