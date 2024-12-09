# Bin mediatorsrv
include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/Util)
include_directories         (${PROJECT_PATH}/3rdAdapter)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Binder)
include_directories         (${PROJECT_PATH}/UtilModules/EpollEventHandler)

aux_source_directory        (${PROJECT_PATH}/Components/Basics/MediatorSrv          SRC_MEDIATOR_SRV_PATH)
add_executable              (BinProxySrv    ${SRC_MEDIATOR_SRV_PATH})
set_target_properties       (BinProxySrv    PROPERTIES OUTPUT_NAME                     "mediatorsrv")
target_link_libraries       (BinProxySrv    rt pthread sprcore sprbinder epollevent)
add_dependencies            (BinProxySrv    LibSprCore LibBinder LibEpollEvent)