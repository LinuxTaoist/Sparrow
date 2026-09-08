# Examples

include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)
include_directories (${PROJECT_PATH}/3rdAdapter)
include_directories (${PROJECT_PATH}/PublicAPIs)
include_directories (${PROJECT_PATH}/PrivateAPIs)
include_directories (${PROJECT_PATH}/UtilModules/EpollEventHandler)

## Bin 01_proc_guard
set(SRC_BIN_01_PROC_GUARD    01_ProcGuard.cc)
set(OTHER_SOURCES           ${PROJECT_PATH}/Util/ProcMutex.cpp)
add_executable              (Bin01ProcGuard ${SRC_BIN_01_PROC_GUARD} ${OTHER_SOURCES})
set_target_properties       (Bin01ProcGuard PROPERTIES OUTPUT_NAME "01_proc_guard")
target_link_libraries       (Bin01ProcGuard c LibSprLog pthread rt)
add_dependencies            (Bin01ProcGuard LibSprLog)

## Bin 02_dir_watch
set(SRC_BIN_02_DIR_WATCH     02_SprDirWatch.cc)
add_executable              (Bin02DirWatch ${SRC_BIN_02_DIR_WATCH})
set_target_properties       (Bin02DirWatch PROPERTIES OUTPUT_NAME "02_dir_watch")
target_link_libraries       (Bin02DirWatch c LibSprCore pthread)
add_dependencies            (Bin02DirWatch LibSprCore)

## Bin 03_user_main
set(SRC_BIN_03_USER_MAIN     03_UserMain.cc)
add_executable              (Bin03UserMain ${SRC_BIN_03_USER_MAIN})
set_target_properties       (Bin03UserMain PROPERTIES OUTPUT_NAME "03_user_main")
target_link_libraries       (Bin03UserMain c LibSprMain pthread rt)
add_dependencies            (Bin03UserMain LibSprMain)

## Bin 04_debug_core
set(SRC_BIN_04_DCORE         04_DebugSprCore.cc)
add_executable              (Bin04DebugCore ${SRC_BIN_04_DCORE})
set_target_properties       (Bin04DebugCore PROPERTIES OUTPUT_NAME "04_debug_core")
target_link_libraries       (Bin04DebugCore c pthread LibSprCore LibPowerManagerAPI)
add_dependencies            (Bin04DebugCore LibSprCore LibPowerManagerAPI)

## Bin 05_debug_msg
set(SRC_BIN_05_DMSG          05_DebugMsg.cc)
add_executable              (Bin05DebugMsg ${SRC_BIN_05_DMSG})
set_target_properties       (Bin05DebugMsg PROPERTIES OUTPUT_NAME "05_debug_msg")
target_link_libraries       (Bin05DebugMsg c pthread LibSprCore)
add_dependencies            (Bin05DebugMsg LibSprCore)

## Bin 06_debug_binder
set(SRC_BIN_06_DBINDER       06_DebugBinder.cc)
add_executable              (Bin06DebugBinder ${SRC_BIN_06_DBINDER})
target_include_directories  (Bin06DebugBinder PRIVATE ${PROJECT_PATH}/PrivateAPIs/Binder)
set_target_properties       (Bin06DebugBinder PROPERTIES OUTPUT_NAME "06_debug_binder")
target_link_libraries       (Bin06DebugBinder c pthread LibBinder)
add_dependencies            (Bin06DebugBinder LibBinder)

## Bin 07_tcp_server
set(SRC_BIN_07_TCPSERVER     07_TcpServer.cc)
add_executable              (Bin07TcpServer ${SRC_BIN_07_TCPSERVER})
target_include_directories  (Bin07TcpServer PRIVATE ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (Bin07TcpServer PROPERTIES OUTPUT_NAME "07_tcp_server")
target_link_libraries       (Bin07TcpServer c LibEpollEvent)
add_dependencies            (Bin07TcpServer LibEpollEvent)

## Bin 08_tcp_client
set(SRC_BIN_08_TCPCLIENT     08_TcpClient.cc)
add_executable              (Bin08TcpClient ${SRC_BIN_08_TCPCLIENT})
target_include_directories  (Bin08TcpClient PRIVATE ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (Bin08TcpClient PROPERTIES OUTPUT_NAME "08_tcp_client")
target_link_libraries       (Bin08TcpClient c LibEpollEvent pthread)
add_dependencies            (Bin08TcpClient LibEpollEvent)

## Bin 09_mqueue
set(SRC_BIN_09_MQUEUE        09_MQueue.cc)
add_executable              (Bin09Mqueue ${SRC_BIN_09_MQUEUE})
set_target_properties       (Bin09Mqueue PROPERTIES OUTPUT_NAME "09_mqueue")
target_link_libraries       (Bin09Mqueue c LibEpollEvent pthread)
add_dependencies            (Bin09Mqueue LibEpollEvent)

## Bin 10_http_bytes
set(SRC_BIN_10_HTTPBYTES     10_HttpBytes.cc)
aux_source_directory        (${PROJECT_PATH}/UtilModules/HTTP SRC_HTTP_SRC_PATH)
add_executable              (Bin10HttpBytes ${SRC_BIN_10_HTTPBYTES} ${SRC_HTTP_SRC_PATH})
target_include_directories  (Bin10HttpBytes PRIVATE ${PROJECT_PATH}/UtilModules/HTTP)
set_target_properties       (Bin10HttpBytes PROPERTIES OUTPUT_NAME "10_http_bytes")
target_link_libraries       (Bin10HttpBytes c rt)

## Bin 11_http_server
set(SRC_BIN_11_HTTPSERVER    11_HttpServer.cc)
aux_source_directory        (${PROJECT_PATH}/UtilModules/HTTP SRC_HSRV_SRC_PATH)
add_executable              (Bin11HttpServer ${SRC_BIN_11_HTTPSERVER} ${SRC_HSRV_SRC_PATH})
target_include_directories  (Bin11HttpServer PRIVATE ${PROJECT_PATH}/UtilModules/HTTP)
set_target_properties       (Bin11HttpServer PROPERTIES OUTPUT_NAME "11_http_server")
target_link_libraries       (Bin11HttpServer c LibEpollEvent rt)
add_dependencies            (Bin11HttpServer LibEpollEvent)

## Bin 12_sqlite
set(SRC_BIN_12_SQLITE        12_Sqlite.cc)
add_executable              (Bin12Sqlite ${SRC_BIN_12_SQLITE})
set_target_properties       (Bin12Sqlite PROPERTIES OUTPUT_NAME "12_sqlite")
target_link_libraries       (Bin12Sqlite c LibSqliteAdapter)
add_dependencies            (Bin12Sqlite LibSqliteAdapter)

## Bin 13_debug_codecx
set(SRC_BIN_13_DCODECX      13_DebugCodecX.cc)
set(DECODECX_UTIL_SOURCES    ${PROJECT_PATH}/Util/GeneralConversions.cpp)
aux_source_directory        (${PROJECT_PATH}/UtilModules/CodecX SRC_CODECX_PATH)
add_executable              (Bin13DebugCodecX ${SRC_BIN_13_DCODECX} ${SRC_CODECX_PATH} ${DECODECX_UTIL_SOURCES})
target_include_directories  (Bin13DebugCodecX PRIVATE ${PROJECT_PATH}/UtilModules/CodecX)
set_target_properties       (Bin13DebugCodecX PROPERTIES OUTPUT_NAME "13_debug_codecx")

## Bin 14_debug_hal
set(SRC_BIN_14_DHAL         14_DebugHwHal.cc)
add_executable              (Bin14DebugHal ${SRC_BIN_14_DHAL})
target_include_directories  (Bin14DebugHal PRIVATE ${PROJECT_PATH}/Hardware/Common/inc)
set_target_properties       (Bin14DebugHal PROPERTIES OUTPUT_NAME "14_debug_hal")
target_link_libraries       (Bin14DebugHal c LibSprHal LibSprCore pthread rt)
add_dependencies            (Bin14DebugHal LibSprHal LibSprCore)
