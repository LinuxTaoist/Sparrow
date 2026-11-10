# TimeManagerSrv

## Bin timemanagersrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/TimeManagerSrv SRC_TIME_MANAGER_PATH)
add_executable              (BinTimeManagerSrv ${SRC_TIME_MANAGER_PATH})
set_target_properties       (BinTimeManagerSrv PROPERTIES OUTPUT_NAME "timemanagersrv")
target_include_directories  (BinTimeManagerSrv PRIVATE ${PROJECT_PATH}/Util
                                                       ${PROJECT_PATH}/Core
                                                       ${PROJECT_PATH}/3rdAdapter
                                                       ${PROJECT_PATH}/PrivateAPIs
                                                       ${PROJECT_PATH}/UtilModules/EpollEventHandler)
target_link_libraries       (BinTimeManagerSrv LibSprCore LibSprMain rt pthread)
add_dependencies            (BinTimeManagerSrv LibSprCore LibSprMain)
