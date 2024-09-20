## Examples

include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)
include_directories (${PROJECT_PATH}/3rdAdapter)
include_directories (${PROJECT_PATH}/PublicAPIs)
include_directories (${PROJECT_PATH}/UtilModules/EpollEventHandler)

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
target_link_libraries       (debug_msg c 3rdadapter pthread sprcore epollevent)
add_dependencies            (debug_msg Lib3rdAdapter LibSprCore LibEpollEvent)

## Bin debugbinder
set(SRC_BIN_DBINDER         DebugBinder.cc)
add_executable              (debug_binder ${SRC_BIN_DBINDER})
target_include_directories  (debug_binder PRIVATE ${PROJECT_PATH}/PrivateAPIs/Binder)
set_target_properties       (debug_binder PROPERTIES OUTPUT_NAME "debugbinder")
target_link_libraries       (debug_binder c sprbinder)
add_dependencies            (debug_binder LibBinder)

## Bin sampleSqlite
# set(SRC_BIN_SAMPLESQLITE    SampleSqlite.cc)
# add_executable              (sample_sqlite ${SRC_BIN_SAMPLESQLITE})
# target_include_directories  (sample_sqlite PRIVATE ${PROJECT_PATH}/3rdParty/sqlite/include)
# set_target_properties       (sample_sqlite PROPERTIES OUTPUT_NAME "sample_sqlite")
# target_link_libraries       (sample_sqlite c 3rdadapter sqlite3 pthread dl)
# add_dependencies            (sample_sqlite Lib3rdAdapter)

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

## Bin property_get
set(SRC_BIN_PROPERTY_GET     GetProperty.cc)
add_executable              (property_get ${SRC_BIN_PROPERTY_GET})
target_include_directories  (property_get PRIVATE ${PROJECT_PATH}/PublicAPIs)
set_target_properties       (property_get PROPERTIES OUTPUT_NAME "property_get")
target_link_libraries       (property_get c propertyapi)
add_dependencies            (property_get LibPropertyAPI)

## Bin property_set
set(SRC_BIN_PROPERTY_SET     SetProperty.cc)
add_executable              (property_set ${SRC_BIN_PROPERTY_SET})
target_include_directories  (property_set PRIVATE ${PROJECT_PATH}/PublicAPIs)
set_target_properties       (property_set PROPERTIES OUTPUT_NAME "property_set")
target_link_libraries       (property_set c propertyapi)
add_dependencies            (property_set LibPropertyAPI)
