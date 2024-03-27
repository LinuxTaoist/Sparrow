## Components

include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)

## copy *.prop
set (GENERAL_CONFIGURE_PATH ${PROJECT_PATH}/Configures/General)
set (VENDOR_CONFIGURE_PATH  ${PROJECT_PATH}/Configures/Vendor/${PROJECT_PLATFORM})

## General configures
file(COPY ${GENERAL_CONFIGURE_PATH}/init.conf       DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/default.prop    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/system.prop     DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)

## Vendor configures
file(COPY ${VENDOR_CONFIGURE_PATH}/vendor.prop      DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)

# Bin logservice
aux_source_directory(${PROJECT_PATH}/Components/LogManagerSrv  SRC_LOG_SRV_PATH)
add_executable       (BinLogSrv ${SRC_LOG_SRV_PATH})
set_target_properties(BinLogSrv PROPERTIES OUTPUT_NAME "logmanagersrv")
target_link_libraries(BinLogSrv sprcore pthread)
add_dependencies     (BinLogSrv libsprcore)

# Bin binderserver
aux_source_directory(${PROJECT_PATH}/Util                   SRC_BINDER_SRV_PATH)
aux_source_directory(${PROJECT_PATH}/Components/BinderSrv   SRC_BINDER_SRV_PATH)
add_executable       (BinBinderSrv ${SRC_BINDER_SRV_PATH})
set_target_properties(BinBinderSrv PROPERTIES OUTPUT_NAME "bindersrv")
target_link_libraries(BinBinderSrv pthread rt)
add_dependencies     (BinBinderSrv librt)

# Bin mediatorsrv
aux_source_directory(${PROJECT_PATH}/Components/MediatorSrv  SRC_PROXY_SRV_PATH)
add_executable       (BinProxySrv ${SRC_PROXY_SRV_PATH})
set_target_properties(BinProxySrv PROPERTIES OUTPUT_NAME "mediatorsrv")
target_link_libraries(BinProxySrv sprcore pthread rt)
add_dependencies     (BinProxySrv libsprcore librt)

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
target_link_libraries(BinPropertySrv sprcore pthread rt)
add_dependencies     (BinPropertySrv libsprcore librt)

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

