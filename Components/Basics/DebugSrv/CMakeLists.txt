# DebugSrv

## Bin debugsrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/DebugSrv DEBUG_SRV_PATH)
add_executable              (BinDebugSrv ${DEBUG_SRV_PATH})
target_include_directories  (BinDebugSrv PRIVATE    ${PROJECT_PATH}/Util
                                                    ${PROJECT_PATH}/Core
                                                    ${PROJECT_PATH}/3rdAdapter
                                                    ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (BinDebugSrv PROPERTIES OUTPUT_NAME "debugsrv")
target_link_libraries       (BinDebugSrv LibSprCore LibBinder pthread dl)
add_dependencies            (BinDebugSrv LibSprCore LibBinder)
