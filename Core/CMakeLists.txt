## Build libprotocol.so

include_directories (${PROJECT_PATH})
include_directories (${PROJECT_PATH}/3rdParty/libgo)
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}    SRC_CORE_PATH)
aux_source_directory(${PROJECT_PATH}/Util           SRC_CORE_PATH)

add_library(LibSprCore SHARED ${SRC_CORE_PATH})
set_target_properties(LibSprCore PROPERTIES OUTPUT_NAME "sprcore")
set_target_properties(LibSprCore PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(LibSprCore stdc++ libgo rt dl)
add_dependencies     (LibSprCore liblibgo librt libdl)