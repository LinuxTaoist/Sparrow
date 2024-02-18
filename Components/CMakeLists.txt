## Components

include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)

# Bin logservice
aux_source_directory(${PROJECT_PATH}/Components/LogManagerSrv  SRC_LOG_SRV_PATH)
add_executable       (BinLogSrv ${SRC_LOG_SRV_PATH})
set_target_properties(BinLogSrv PROPERTIES OUTPUT_NAME "logmanagersrv")
target_link_libraries(BinLogSrv sprcore pthread)
add_dependencies     (BinLogSrv libsprcore)

# Bin mediatorsrv
aux_source_directory(${PROJECT_PATH}/Components/MediatorSrv  SRC_PROXY_SRV_PATH)
add_executable       (BinProxySrv ${SRC_PROXY_SRV_PATH})
set_target_properties(BinProxySrv PROPERTIES OUTPUT_NAME "mediatorsrv")
target_link_libraries(BinProxySrv sprcore pthread rt)
add_dependencies     (BinProxySrv libsprcore)

# Bin powermanagersrv
aux_source_directory(${PROJECT_PATH}/Components/PowerManagerSrv  SRC_POWER_MANAGER_PATH)
add_executable       (BinPowerManagerSrv ${SRC_POWER_MANAGER_PATH})
set_target_properties(BinPowerManagerSrv PROPERTIES OUTPUT_NAME "powermanagersrv")
target_link_libraries(BinPowerManagerSrv sprcore pthread)
add_dependencies     (BinPowerManagerSrv libsprcore)

# Bin propertiessrv
aux_source_directory(${PROJECT_PATH}/Util                        SRC_PROPERTY_SRV_PATH)
aux_source_directory(${PROJECT_PATH}/Components/PropertiesSrv    SRC_PROPERTY_SRV_PATH)
add_executable       (BinPropertySrv ${SRC_PROPERTY_SRV_PATH})
set_target_properties(BinPropertySrv PROPERTIES OUTPUT_NAME "propertiessrv")
target_link_libraries(BinPropertySrv pthread)

# Bin servicemanagersrv
aux_source_directory(${PROJECT_PATH}/Components/ServiceManagerSrv  SRC_SRV_PATH)
add_executable       (BinSrvMgr ${SRC_SRV_PATH})
set_target_properties(BinSrvMgr PROPERTIES OUTPUT_NAME "servicemanagersrv")
target_link_libraries(BinSrvMgr pthread)

# Bin sparrowsrv
aux_source_directory(${PROJECT_PATH}/Components/SparrowSrv  SRC_MAIN_SPR_PATH)
add_executable       (BinMainSpr ${SRC_MAIN_SPR_PATH})
set_target_properties(BinMainSpr PROPERTIES OUTPUT_NAME "sparrowsrv")
target_link_libraries(BinMainSpr sprcore)
add_dependencies     (BinMainSpr libsprcore)
