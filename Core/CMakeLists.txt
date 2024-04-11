## Build libsprcore.so

include_directories (${PROJECT_PATH}/Util)
include_directories (${PROJECT_PATH}/3rdParty/libgo/include)
include_directories (${PROJECT_PATH}/3rdAdapter)
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}    SRC_CORE_PATH)
aux_source_directory(${PROJECT_PATH}/Util           SRC_CORE_PATH)

add_library(LibSprCore SHARED ${SRC_CORE_PATH})
set_target_properties(LibSprCore PROPERTIES OUTPUT_NAME "sprcore")
set_target_properties(LibSprCore PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(LibSprCore stdc++ 3rdadapter libgo rt dl)
add_dependencies     (LibSprCore Lib3rdAdapter)