## Examples

include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)
include_directories (${PROJECT_PATH}/3rdAdapter)
include_directories (${PROJECT_PATH}/PublicAPIs)
include_directories (${PROJECT_PATH}/UtilModules/EpollEventHandler)

## Bin 01_procguard
set(SRC_BIN_01_PROC_GUARD    01_ProcGuard.cc)
set(OTHER_SOURCES           ${PROJECT_PATH}/Util/ProcMutex.cpp)
add_executable              (01_proc_guard ${SRC_BIN_01_PROC_GUARD} ${OTHER_SOURCES})
set_target_properties       (01_proc_guard PROPERTIES OUTPUT_NAME "01_proc_guard")
target_link_libraries       (01_proc_guard c sprlog pthread rt)
add_dependencies            (01_proc_guard LibSprLog)

## Bin 02_dirwatch
set(SRC_BIN_02_DIR_WATCH     02_SprDirWatch.cc)
add_executable              (02_dir_watch ${SRC_BIN_02_DIR_WATCH} ${SRC_DIR_WATCH})
set_target_properties       (02_dir_watch PROPERTIES OUTPUT_NAME "02_dir_watch")
target_link_libraries       (02_dir_watch c sprcore epollevent pthread)
add_dependencies            (02_dir_watch LibSprCore LibEpollEvent)

## Bin debugcore
set(SRC_BIN_DCORE           DebugSprCore.cc)
add_executable              (debug_core ${SRC_BIN_DCORE})
set_target_properties       (debug_core PROPERTIES OUTPUT_NAME "debugcore")
target_link_libraries       (debug_core c pthread sprcore epollevent powermanagerapi)
add_dependencies            (debug_core LibSprCore LibPowerManagerAPI LibEpollEvent)

## Bin debugmsg
set(SRC_BIN_DMSG            DebugMsg.cc)
add_executable              (debug_msg ${SRC_BIN_DMSG})
set_target_properties       (debug_msg PROPERTIES OUTPUT_NAME "debugmsg")
target_link_libraries       (debug_msg c sprlog pthread sprcore epollevent)
add_dependencies            (debug_msg LibSprLog LibSprCore LibEpollEvent)

## Bin debugbinder
set(SRC_BIN_DBINDER         DebugBinder.cc)
add_executable              (debug_binder ${SRC_BIN_DBINDER})
target_include_directories  (debug_binder PRIVATE ${PROJECT_PATH}/PrivateAPIs/Binder)
set_target_properties       (debug_binder PROPERTIES OUTPUT_NAME "debugbinder")
target_link_libraries       (debug_binder c pthread sprbinder)
add_dependencies            (debug_binder LibBinder)

## Bin sampleSqlite
# set(SRC_BIN_SAMPLESQLITE    SampleSqlite.cc)
# add_executable              (sample_sqlite ${SRC_BIN_SAMPLESQLITE})
# target_include_directories  (sample_sqlite PRIVATE ${PROJECT_PATH}/3rdParty/sqlite/include)
# set_target_properties       (sample_sqlite PROPERTIES OUTPUT_NAME "sample_sqlite")
# target_link_libraries       (sample_sqlite c sprlog sqlite3 pthread dl)
# add_dependencies            (sample_sqlite LibSprLog)

## Bin sampleTcpServer
set(SRC_BIN_STCPSERVER      SampleTcpServer.cc)
add_executable              (sample_tcp_server ${SRC_BIN_STCPSERVER})
target_include_directories  (sample_tcp_server PRIVATE ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (sample_tcp_server PROPERTIES OUTPUT_NAME "sample_tcpserver")
target_link_libraries       (sample_tcp_server c epollevent)
add_dependencies            (sample_tcp_server LibEpollEvent)

## Bin sampleTcpClient
set(SRC_BIN_STCPCLIENT      SampleTcpClient.cc)
add_executable              (sample_tcp_client ${SRC_BIN_STCPCLIENT})
target_include_directories  (sample_tcp_client PRIVATE ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (sample_tcp_client PROPERTIES OUTPUT_NAME "sample_tcpclient")
target_link_libraries       (sample_tcp_client c epollevent pthread)
add_dependencies            (sample_tcp_client LibEpollEvent)

## Bin sampleMQueue
set(SRC_BIN_STCMQUEUE       SampleMQueue.cc)
add_executable              (sample_mqueue ${SRC_BIN_STCMQUEUE})
set_target_properties       (sample_mqueue PROPERTIES OUTPUT_NAME "sample_mqueue")
target_link_libraries       (sample_mqueue c epollevent pthread)
add_dependencies            (sample_mqueue LibEpollEvent)

## Bin getprop
set(SRC_BIN_PROPERTY_GET    GetProperty.cc)
add_executable              (property_get ${SRC_BIN_PROPERTY_GET})
target_include_directories  (property_get PRIVATE ${PROJECT_PATH}/PublicAPIs)
set_target_properties       (property_get PROPERTIES OUTPUT_NAME "getprop")
target_link_libraries       (property_get c propertyapi)
add_dependencies            (property_get LibPropertyAPI)

## Bin setprop
set(SRC_BIN_PROPERTY_SET    SetProperty.cc)
add_executable              (property_set ${SRC_BIN_PROPERTY_SET})
target_include_directories  (property_set PRIVATE ${PROJECT_PATH}/PublicAPIs)
set_target_properties       (property_set PROPERTIES OUTPUT_NAME "setprop")
target_link_libraries       (property_set c propertyapi)
add_dependencies            (property_set LibPropertyAPI)

## Bin sample_http_bytes
aux_source_directory        (${PROJECT_PATH}/UtilModules/HTTP SRC_HTTP_SRC_PATH)
set(SRC_BIN_HTTPBYTES       SampleHttpBytes.cc)
add_executable              (sample_http_bytes ${SRC_BIN_HTTPBYTES} ${SRC_HTTP_SRC_PATH})
target_include_directories  (sample_http_bytes PRIVATE ${PROJECT_PATH}/UtilModules/HTTP)
set_target_properties       (sample_http_bytes PROPERTIES OUTPUT_NAME "sample_http_bytes")
target_link_libraries       (sample_http_bytes c rt)

## Bin sample_httpserver
aux_source_directory        (${PROJECT_PATH}/UtilModules/HTTP SRC_HTTP_SRC_PATH)
set(SRC_BIN_HTTPSERVER      SampleHttpServer.cc)
add_executable              (sample_http_server ${SRC_BIN_HTTPSERVER} ${SRC_HTTP_SRC_PATH})
target_include_directories  (sample_tcp_server PRIVATE ${PROJECT_PATH}/UtilModules/EpollEventHandler)
target_include_directories  (sample_http_server PRIVATE ${PROJECT_PATH}/UtilModules/HTTP)
set_target_properties       (sample_http_server PROPERTIES OUTPUT_NAME "sample_httpserver")
target_link_libraries       (sample_http_server c epollevent rt)
add_dependencies            (sample_http_server LibEpollEvent)
