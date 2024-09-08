# Bin servicemanagersrv
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/3rdAdapter)
include_directories     (${PROJECT_PATH}/Core)

aux_source_directory        (${PROJECT_PATH}/Util                                   SRC_SRVICE_MANAGER_PATH)
aux_source_directory        (${PROJECT_PATH}/Components/Basics/ServiceManagerSrv    SRC_SRVICE_MANAGER_PATH)
add_executable              (BinSrvMgr ${SRC_SRVICE_MANAGER_PATH})
set_target_properties       (BinSrvMgr PROPERTIES OUTPUT_NAME                       "servicemanagersrv")
target_link_libraries       (BinSrvMgr pthread 3rdadapter)
add_dependencies            (BinSrvMgr Lib3rdAdapter)