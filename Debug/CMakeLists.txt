## Debug
set(DEBUG_PATH ${PROJECT_PATH}/Debug)

include_directories (${PROJECT_PATH}/Util)

# Bin logshow
set(SRC_BIN_LOGSHOW         ${DEBUG_PATH}/LogShow/main_logshow.cpp)
aux_source_directory        (${PROJECT_PATH}/Util SRC_OTHER_PATH)
add_executable              (BinLogShow ${SRC_BIN_LOGSHOW} ${SRC_OTHER_PATH})
set_target_properties       (BinLogShow PROPERTIES OUTPUT_NAME "logshow")
target_link_libraries       (BinLogShow pthread)

# Bin monitorsrv
# aux_source_directory(${PROJECT_PATH}/Components/MediatorSrv  SRC_MONITOR_SRV_PATH)
# add_executable              (BinMonitorSrv ${SRC_MONITOR_SRV_PATH})
# set_target_properties       (BinMonitorSrv PROPERTIES OUTPUT_NAME "monitorsrv")
# target_link_libraries       (BinMonitorSrv rt pthread sprcore)
# add_dependencies            (BinMonitorSrv LibSprCore)
