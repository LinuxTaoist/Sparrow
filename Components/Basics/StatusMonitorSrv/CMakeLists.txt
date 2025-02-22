# Bin mediatorsrv
include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/Util)
include_directories         (${PROJECT_PATH}/3rdAdapter)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Binder)
include_directories         (${PROJECT_PATH}/UtilModules/EpollEventHandler)

aux_source_directory        (${PROJECT_PATH}/Components/Basics/StatusMonitorSrv     SRC_MONITOR_SRV_PATH)
add_executable              (BinStatusMonitor   ${SRC_MONITOR_SRV_PATH})
set_target_properties       (BinStatusMonitor   PROPERTIES OUTPUT_NAME              "statusmonitorsrv")
target_link_libraries       (BinStatusMonitor   rt pthread sprcore sprbinder epollevent)
add_dependencies            (BinStatusMonitor   LibSprCore LibBinder LibEpollEvent)