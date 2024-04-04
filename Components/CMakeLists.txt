## Components

include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)
include_directories (${PROJECT_PATH}/Adapter)

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
add_executable              (BinLogSrv ${SRC_LOG_SRV_PATH})
set_target_properties       (BinLogSrv PROPERTIES OUTPUT_NAME "logmanagersrv")
target_link_libraries       (BinLogSrv pthread sprcore)
add_dependencies            (BinLogSrv LibSprCore)

# Bin mediatorsrv
aux_source_directory(${PROJECT_PATH}/Components/MediatorSrv  SRC_PROXY_SRV_PATH)
add_executable              (BinProxySrv ${SRC_PROXY_SRV_PATH})
set_target_properties       (BinProxySrv PROPERTIES OUTPUT_NAME "mediatorsrv")
target_link_libraries       (BinProxySrv rt pthread sprcore)
add_dependencies            (BinProxySrv LibSprCore)

# Bin powermanagersrv
aux_source_directory(${PROJECT_PATH}/Components/PowerManagerSrv  SRC_POWER_MANAGER_PATH)
add_executable              (BinPowerManagerSrv ${SRC_POWER_MANAGER_PATH})
set_target_properties       (BinPowerManagerSrv PROPERTIES OUTPUT_NAME "powermanagersrv")
target_link_libraries       (BinPowerManagerSrv pthread sprcore spradapter)
add_dependencies            (BinPowerManagerSrv LibSprCore LibSprAdapter)

# Bin propertiessrv
aux_source_directory        (${PROJECT_PATH}/Util                        SRC_PROPERTY_SRV_PATH)
aux_source_directory        (${PROJECT_PATH}/Components/PropertiesSrv    SRC_PROPERTY_SRV_PATH)
add_executable              (BinPropertySrv ${SRC_PROPERTY_SRV_PATH})
target_include_directories  (BinPropertySrv PRIVATE ${PROJECT_PATH}/Components/BinderSrv/Library)
set_target_properties       (BinPropertySrv PROPERTIES OUTPUT_NAME "propertiessrv")
target_link_libraries       (BinPropertySrv pthread rt sprcore sprbinder spradapter)
add_dependencies            (BinPropertySrv LibSprCore LibBinder LibSprAdapter)

# Bin servicemanagersrv
aux_source_directory(${PROJECT_PATH}/Components/ServiceManagerSrv  SRC_SRV_PATH)
add_executable              (BinSrvMgr ${SRC_SRV_PATH})
set_target_properties       (BinSrvMgr PROPERTIES OUTPUT_NAME "servicemanagersrv")
target_link_libraries       (BinSrvMgr pthread spradapter)
add_dependencies            (BinSrvMgr LibSprAdapter)

# Bin sparrowsrv
aux_source_directory        (${PROJECT_PATH}/Components/SparrowSrv  SRC_MAIN_SPR_PATH)
add_executable              (BinMainSpr ${SRC_MAIN_SPR_PATH})
set_target_properties       (BinMainSpr PROPERTIES OUTPUT_NAME "sparrowsrv")
target_link_libraries       (BinMainSpr sprcore)
add_dependencies            (BinMainSpr LibSprCore)

add_subdirectory(BinderSrv)
