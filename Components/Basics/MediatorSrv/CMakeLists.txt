## Bin mediatorsrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/MediatorSrv SRC_MEDIATOR_SRV_PATH)
add_executable              (BinProxySrv    ${SRC_MEDIATOR_SRV_PATH})
target_include_directories  (BinProxySrv    PRIVATE ${PROJECT_PATH}/Core
                                            ${PROJECT_PATH}/Util
                                            ${PROJECT_PATH}/3rdAdapter
                                            ${PROJECT_PATH}/PrivateAPIs/Binder
                                            ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (BinProxySrv    PROPERTIES OUTPUT_NAME "mediatorsrv")
target_link_libraries       (BinProxySrv    rt pthread sprcore sprbinder epollevent sprlog)
add_dependencies            (BinProxySrv    LibSprCore LibBinder LibEpollEvent LibSprLog)