## Debug
include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)
include_directories (${PROJECT_PATH}/Adapter)

# Bin logshow
aux_source_directory(${PROJECT_PATH}/Components/LogManagerSrv  SRC_LOG_SHOW_PATH)
add_executable              (BinLogShow ${SRC_LOG_SHOW_PATH})
set_target_properties       (BinLogShow PROPERTIES OUTPUT_NAME "logshow")
target_link_libraries       (BinLogShow pthread sprcore)
add_dependencies            (BinLogShow LibSprCore)

# Bin monitorsrv
aux_source_directory(${PROJECT_PATH}/Components/MediatorSrv  SRC_MONITOR_SRV_PATH)
add_executable              (BinMonitorSrv ${SRC_MONITOR_SRV_PATH})
set_target_properties       (BinMonitorSrv PROPERTIES OUTPUT_NAME "monitorsrv")
target_link_libraries       (BinMonitorSrv rt pthread sprcore)
add_dependencies            (BinMonitorSrv LibSprCore)
