## 3rdAdapter
include_directories         (${PROJECT_PATH}/Core)
set(SPRLOG_SRC_FILES         ${PROJECT_PATH}/Util/SharedRingBuffer.cpp
                             ${PROJECT_PATH}/3rdAdapter/SprLog.cpp)

## Build libspradapter.so
add_library                 (LibSprLog SHARED ${SPRLOG_SRC_FILES})
target_include_directories  (LibSprLog PRIVATE ${PROJECT_PATH}/Util)
target_include_directories  (LibSprLog PRIVATE ${PROJECT_PATH}/3rdadapter)
set_target_properties       (LibSprLog PROPERTIES OUTPUT_NAME "sprlog")
set_target_properties       (LibSprLog PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibSprLog stdc++ rt dl)
