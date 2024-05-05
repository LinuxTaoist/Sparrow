## 3rdAdapter
include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/3rdAdapter)
include_directories         (${PROJECT_PATH}/3rdParty/libgo/include)
include_directories         (${PROJECT_PATH}/3rdParty/sqlite/include)
aux_source_directory        (${PROJECT_PATH}/3rdAdapter     SRC_ADAPTER_PATH)
set(OTHER_SRC_FILES         ${PROJECT_PATH}/Util/SharedRingBuffer.cpp)

## Build libspradapter.so
add_library                 (Lib3rdAdapter STATIC ${SRC_ADAPTER_PATH} ${OTHER_SRC_FILES})
target_include_directories  (Lib3rdAdapter PRIVATE ${PROJECT_PATH}/Util)
set_target_properties       (Lib3rdAdapter PROPERTIES OUTPUT_NAME "3rdadapter")
set_target_properties       (Lib3rdAdapter PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (Lib3rdAdapter stdc++ rt dl libgo sqlite3)
