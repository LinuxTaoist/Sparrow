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
add_executable              (proc_guard ${SRC_BIN_01_PROC_GUARD} ${OTHER_SOURCES})
set_target_properties       (proc_guard PROPERTIES OUTPUT_NAME "01_proc_guard")
target_link_libraries       (proc_guard c LibSprLog pthread rt)
add_dependencies            (proc_guard LibSprLog)

## Bin 02_dir_watch
set(SRC_BIN_02_DIR_WATCH     02_SprDirWatch.cc)
add_executable              (dir_watch ${SRC_BIN_02_DIR_WATCH})
set_target_properties       (dir_watch PROPERTIES OUTPUT_NAME "02_dir_watch")
target_link_libraries       (dir_watch c LibSprCore pthread)
add_dependencies            (dir_watch LibSprCore)

## Bin 03_user_main
set(SRC_BIN_03_USER_MAIN     03_UserMain.cc)
add_executable              (user_main ${SRC_BIN_03_USER_MAIN})
set_target_properties       (user_main PROPERTIES OUTPUT_NAME "03_user_main")
target_link_libraries       (user_main c LibSprMain pthread rt)
add_dependencies            (user_main LibSprMain)

## Bin 04_debug_core
set(SRC_BIN_04_DCORE         04_DebugSprCore.cc)
add_executable              (debug_core ${SRC_BIN_04_DCORE})
set_target_properties       (debug_core PROPERTIES OUTPUT_NAME "04_debug_core")
target_link_libraries       (debug_core c pthread LibSprCore LibPowerManagerAPI)
add_dependencies            (debug_core LibSprCore LibPowerManagerAPI)

## Bin 05_debug_msg
set(SRC_BIN_05_DMSG          05_DebugMsg.cc)
add_executable              (debug_msg ${SRC_BIN_05_DMSG})
set_target_properties       (debug_msg PROPERTIES OUTPUT_NAME "05_debug_msg")
target_link_libraries       (debug_msg c pthread LibSprCore)
add_dependencies            (debug_msg LibSprCore)

## Bin 06_debug_binder
set(SRC_BIN_06_DBINDER       06_DebugBinder.cc)
add_executable              (debug_binder ${SRC_BIN_06_DBINDER})
target_include_directories  (debug_binder PRIVATE ${PROJECT_PATH}/PrivateAPIs/Binder)
set_target_properties       (debug_binder PROPERTIES OUTPUT_NAME "06_debug_binder")
target_link_libraries       (debug_binder c pthread LibBinder)
add_dependencies            (debug_binder LibBinder)

## Bin 07_tcp_server
set(SRC_BIN_07_TCPSERVER     07_TcpServer.cc)
add_executable              (tcp_server ${SRC_BIN_07_TCPSERVER})
target_include_directories  (tcp_server PRIVATE ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (tcp_server PROPERTIES OUTPUT_NAME "07_tcp_server")
target_link_libraries       (tcp_server c LibEpollEvent)
add_dependencies            (tcp_server LibEpollEvent)

## Bin 08_tcp_client
set(SRC_BIN_08_TCPCLIENT     08_TcpClient.cc)
add_executable              (tcp_client ${SRC_BIN_08_TCPCLIENT})
target_include_directories  (tcp_client PRIVATE ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (tcp_client PROPERTIES OUTPUT_NAME "08_tcp_client")
target_link_libraries       (tcp_client c LibEpollEvent pthread)
add_dependencies            (tcp_client LibEpollEvent)

## Bin 09_mqueue
set(SRC_BIN_09_MQUEUE        09_MQueue.cc)
add_executable              (mqueue ${SRC_BIN_09_MQUEUE})
set_target_properties       (mqueue PROPERTIES OUTPUT_NAME "09_mqueue")
target_link_libraries       (mqueue c LibEpollEvent pthread)
add_dependencies            (mqueue LibEpollEvent)

## Bin 10_http_bytes
set(SRC_BIN_10_HTTPBYTES     10_HttpBytes.cc)
aux_source_directory        (${PROJECT_PATH}/UtilModules/HTTP SRC_HTTP_SRC_PATH)
add_executable              (http_bytes ${SRC_BIN_10_HTTPBYTES} ${SRC_HTTP_SRC_PATH})
target_include_directories  (http_bytes PRIVATE ${PROJECT_PATH}/UtilModules/HTTP)
set_target_properties       (http_bytes PROPERTIES OUTPUT_NAME "10_http_bytes")
target_link_libraries       (http_bytes c rt)

## Bin 11_http_server
set(SRC_BIN_11_HTTPSERVER    11_HttpServer.cc)
aux_source_directory        (${PROJECT_PATH}/UtilModules/HTTP SRC_HSRV_SRC_PATH)
add_executable              (http_server ${SRC_BIN_11_HTTPSERVER} ${SRC_HSRV_SRC_PATH})
target_include_directories  (http_server PRIVATE ${PROJECT_PATH}/UtilModules/HTTP)
set_target_properties       (http_server PROPERTIES OUTPUT_NAME "11_http_server")
target_link_libraries       (http_server c LibEpollEvent rt)
add_dependencies            (http_server LibEpollEvent)

## Bin 13_debug_codecx
set(SRC_BIN_13_DCODECX      13_DebugCodecX.cc)
set(DECODECX_UTIL_SOURCES    ${PROJECT_PATH}/Util/GeneralConversions.cpp)
aux_source_directory        (${PROJECT_PATH}/UtilModules/CodecX SRC_CODECX_PATH)
add_executable              (debug_codecx ${SRC_BIN_13_DCODECX} ${SRC_CODECX_PATH} ${DECODECX_UTIL_SOURCES})
target_include_directories  (debug_codecx PRIVATE ${PROJECT_PATH}/UtilModules/CodecX)
set_target_properties       (debug_codecx PROPERTIES OUTPUT_NAME "13_debug_codecx")

## Bin 14_debug_hal
set(SRC_BIN_14_DHAL         14_DebugHwHal.cc)
add_executable              (debug_hal ${SRC_BIN_14_DHAL})
target_include_directories  (debug_hal PRIVATE ${PROJECT_PATH}/Hardware/Common/inc)
set_target_properties       (debug_hal PROPERTIES OUTPUT_NAME "14_debug_hal")
target_link_libraries       (debug_hal c LibSprHal LibSprCore pthread rt)
add_dependencies            (debug_hal LibSprHal LibSprCore)
