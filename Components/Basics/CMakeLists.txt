## Basics

include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/Util)
include_directories         (${PROJECT_PATH}/3rdAdapter)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Binder)
include_directories         (${PROJECT_PATH}/UtilModules/EpollEventHandler)

## Project configures path
set (GENERAL_CONFIGURE_PATH ${PROJECT_PATH}/ProjectConfigs/General)
set (VENDOR_CONFIGURE_PATH  ${PROJECT_PATH}/ProjectConfigs/Vendor/${PROJECT_PLATFORM})

## Project general configures
file(COPY ${GENERAL_CONFIGURE_PATH}/default.prop    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/init.conf       DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/sprlog.conf     DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/system.prop     DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)

## Project vendor configures
file(COPY ${VENDOR_CONFIGURE_PATH}/vendor.prop      DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)

# Bin binderserver
aux_source_directory        (${PROJECT_PATH}/Util                                   SRC_BINDER_SRV_PATH)
aux_source_directory        (${PROJECT_PATH}/Components/Basics/BinderManagerSrv     SRC_BINDER_SRV_PATH)
add_executable              (BinBinderSrv ${SRC_BINDER_SRV_PATH})
set_target_properties       (BinBinderSrv PROPERTIES OUTPUT_NAME                    "bindermanagersrv")
target_link_libraries       (BinBinderSrv pthread rt 3rdadapter)
add_dependencies            (BinBinderSrv Lib3rdAdapter)

# Bin logservice
aux_source_directory        (${PROJECT_PATH}/Components/Basics/LogManagerSrv        SRC_LOG_SRV_PATH)
add_executable              (BinLogSrv ${SRC_LOG_SRV_PATH})
set_target_properties       (BinLogSrv PROPERTIES OUTPUT_NAME                       "logmanagersrv")
target_link_libraries       (BinLogSrv pthread sprcore)
add_dependencies            (BinLogSrv LibSprCore)

# Bin mediatorsrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/MediatorSrv          SRC_MEDIATOR_SRV_PATH)
add_executable              (BinProxySrv ${SRC_MEDIATOR_SRV_PATH})
set_target_properties       (BinProxySrv PROPERTIES OUTPUT_NAME                     "mediatorsrv")
target_link_libraries       (BinProxySrv rt pthread sprcore sprbinder)
add_dependencies            (BinProxySrv LibSprCore LibBinder)

# Bin powermanagersrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/PowerManagerSrv      SRC_POWER_MANAGER_PATH)
add_executable              (BinPowerManagerSrv ${SRC_POWER_MANAGER_PATH})
set_target_properties       (BinPowerManagerSrv PROPERTIES OUTPUT_NAME              "powermanagersrv")
target_link_libraries       (BinPowerManagerSrv pthread sprcore 3rdadapter)
add_dependencies            (BinPowerManagerSrv LibSprCore Lib3rdAdapter)

# Bin propertiessrv
aux_source_directory        (${PROJECT_PATH}/Util                                   SRC_PROPERTIES_SRV_PATH)
aux_source_directory        (${PROJECT_PATH}/Components/Basics/PropertiesSrv        SRC_PROPERTIES_SRV_PATH)
add_executable              (BinPropertySrv ${SRC_PROPERTIES_SRV_PATH})
set_target_properties       (BinPropertySrv PROPERTIES OUTPUT_NAME                  "propertiessrv")
target_link_libraries       (BinPropertySrv pthread rt sprcore sprbinder 3rdadapter)
add_dependencies            (BinPropertySrv LibSprCore LibBinder Lib3rdAdapter)

# Bin servicemanagersrv
aux_source_directory        (${PROJECT_PATH}/Util                                   SRC_SRVICE_MANAGER_PATH)
aux_source_directory        (${PROJECT_PATH}/Components/Basics/ServiceManagerSrv    SRC_SRVICE_MANAGER_PATH)
add_executable              (BinSrvMgr ${SRC_SRVICE_MANAGER_PATH})
set_target_properties       (BinSrvMgr PROPERTIES OUTPUT_NAME                       "servicemanagersrv")
target_link_libraries       (BinSrvMgr pthread 3rdadapter)
add_dependencies            (BinSrvMgr Lib3rdAdapter)

# Bin sparrowsrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/SparrowSrv           SRC_SPARROW_SRV_PATH)
add_executable              (BinMainSpr ${SRC_SPARROW_SRV_PATH})
set_target_properties       (BinMainSpr PROPERTIES OUTPUT_NAME                      "sparrowsrv")
target_link_libraries       (BinMainSpr sprcore)
add_dependencies            (BinMainSpr LibSprCore)

# Bin debugsrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/DebugSrv             DEBUG_SRV_PATH)
aux_source_directory        (${PROJECT_PATH}/Core/EpollEventHandler                 DEBUG_SRV_PATH)
add_executable              (BinDebugSrv ${DEBUG_SRV_PATH})
set_target_properties       (BinDebugSrv PROPERTIES OUTPUT_NAME                     "debugsrv")
target_link_libraries       (BinDebugSrv sprcore sprbinder epollevent pthread)
add_dependencies            (BinDebugSrv LibSprCore LibEpollEvent LibBinder)
