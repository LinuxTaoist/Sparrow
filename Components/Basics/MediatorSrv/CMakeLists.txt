## MediatorSrv

# Bin mediatorsrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/MediatorSrv SRC_MEDIATOR_SRV_PATH)
add_executable              (BinMediatorSrv ${SRC_MEDIATOR_SRV_PATH})
target_include_directories  (BinMediatorSrv PRIVATE ${PROJECT_PATH}/Core
                                            ${PROJECT_PATH}/Util
                                            ${PROJECT_PATH}/3rdAdapter
                                            ${PROJECT_PATH}/PrivateAPIs/Binder
                                            ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (BinMediatorSrv PROPERTIES OUTPUT_NAME "mediatorsrv")
target_link_libraries       (BinMediatorSrv rt pthread LibSprCore LibBinder)
add_dependencies            (BinMediatorSrv LibSprCore LibBinder)