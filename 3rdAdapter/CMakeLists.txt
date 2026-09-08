# 3rdAdapter
set(SPRLOG_SRC_FILES        ${PROJECT_PATH}/Util/ProcMutex.cpp
                            ${PROJECT_PATH}/Util/SharedRingBuffer.cpp
                            ${PROJECT_PATH}/3rdAdapter/SprLog.cpp)

set(SQLITE_ADAPTER_SRC_FILES ${PROJECT_PATH}/3rdAdapter/SqliteAdapter.cpp)

## Build libspradapter.so
add_library                 (LibSprLog SHARED  ${SPRLOG_SRC_FILES})
target_include_directories  (LibSprLog PRIVATE ${PROJECT_PATH}/Core
                                               ${PROJECT_PATH}/Util
                                               ${PROJECT_PATH}/3rdAdapter)
set_target_properties       (LibSprLog PROPERTIES OUTPUT_NAME "sprlog")
set_target_properties       (LibSprLog PROPERTIES LINKER_LANGUAGE CXX)

## Build libsqladapter.so
add_library                 (LibSqliteAdapter SHARED ${SQLITE_ADAPTER_SRC_FILES})
target_include_directories  (LibSqliteAdapter PRIVATE ${PROJECT_PATH}/3rdAdapter
                                                      ${PROJECT_PATH}/3rdParty/sqlite/include)
set_target_properties       (LibSqliteAdapter PROPERTIES OUTPUT_NAME "sqladapter")
set_target_properties       (LibSqliteAdapter PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibSqliteAdapter PRIVATE sqlite3 pthread rt)

