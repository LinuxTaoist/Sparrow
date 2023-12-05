##############################################################################
include_directories (${PROJECT_PATH}/Core)

# Bin proxyservice
aux_source_directory(${PROJECT_PATH}/Components/ProxyService  SRC_PROXY_SRV_PATH)
add_executable       (BinProxySrv ${SRC_PROXY_SRV_PATH})
set_target_properties(BinProxySrv PROPERTIES OUTPUT_NAME "proxyservice")
target_link_libraries(BinProxySrv sprcore pthread)
add_dependencies     (BinProxySrv libsprcore)

# Bin mainsparrow
aux_source_directory(${PROJECT_PATH}/Components/MainSparrow  SRC_MAIN_SPR_PATH)

add_executable       (BinMainSpr ${SRC_MAIN_SPR_PATH})
set_target_properties(BinMainSpr PROPERTIES OUTPUT_NAME "sparrow")
target_link_libraries(BinMainSpr sprcore)
add_dependencies     (BinMainSpr libsprcore)

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
