# ServiceManagerSrv

## Bin servicemanagersrv
set(OTHER_SRC_FILES         ${PROJECT_PATH}/Util/GeneralUtils.cpp)
aux_source_directory        (${PROJECT_PATH}/Components/Basics/ServiceManagerSrv    SRC_SRVICE_MANAGER_PATH)
add_executable              (BinSrvMgr ${SRC_SRVICE_MANAGER_PATH} ${OTHER_SRC_FILES})
target_include_directories  (BinSrvMgr PRIVATE  ${PROJECT_PATH}/Util
                                                ${PROJECT_PATH}/3rdAdapter
                                                ${PROJECT_PATH}/Core)
set_target_properties       (BinSrvMgr PROPERTIES OUTPUT_NAME                       "servicemanagersrv")
target_link_libraries       (BinSrvMgr pthread LibSprLog rt c dl)
add_dependencies            (BinSrvMgr LibSprLog)