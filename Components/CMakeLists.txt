##############################################################################
include_directories (${PROJECT_PATH}/Core)

# Bin logservice
aux_source_directory(${PROJECT_PATH}/Components/LogManager  SRC_LOG_SRV_PATH)

add_executable       (BinLogSrv ${SRC_LOG_SRV_PATH})
set_target_properties(BinLogSrv PROPERTIES OUTPUT_NAME "logservice")
target_link_libraries(BinLogSrv sprcore pthread)
add_dependencies     (BinLogSrv libsprcore)

# Bin servicemanager
aux_source_directory(${PROJECT_PATH}/Components/ServiceManager  SRC_SRV_PATH)

add_executable       (BinSrvMgr ${SRC_SRV_PATH})
set_target_properties(BinSrvMgr PROPERTIES OUTPUT_NAME "servicemanager")
target_link_libraries(BinSrvMgr pthread)
