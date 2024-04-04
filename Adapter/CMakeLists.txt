## Build libspradapter.so

include_directories (${PROJECT_PATH}/Adapter)
include_directories (${PROJECT_PATH}/3rdParty/libgo/include)
aux_source_directory(${PROJECT_PATH}/Adapter        SRC_ADAPTER_PATH)
set(OTHER_SRC_FILES ${PROJECT_PATH}/Util/SharedRingBuffer.cpp)

add_library                 (LibSprAdapter STATIC ${SRC_ADAPTER_PATH} ${OTHER_SRC_FILES})
target_include_directories  (LibSprAdapter PRIVATE ${PROJECT_PATH}/Util)
set_target_properties       (LibSprAdapter PROPERTIES OUTPUT_NAME "spradapter")
set_target_properties       (LibSprAdapter PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibSprAdapter stdc++ rt dl libgo)
