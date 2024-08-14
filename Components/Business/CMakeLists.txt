## Business
include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/3rdAdapter)
include_directories (${PROJECT_PATH}/UtilModules/EpollEventHandler)

# Bin onenetserver
aux_source_directory        (${PROJECT_PATH}/Components/Business/OneNetMqtt         SRC_ONENET_SRV_PATH)
add_executable              (BinOneNetSrv ${SRC_ONENET_SRV_PATH})
set_target_properties       (BinOneNetSrv PROPERTIES OUTPUT_NAME                    "onenetmanagersrv")
target_link_libraries       (BinOneNetSrv pthread rt 3rdadapter sprcore epollevent)
add_dependencies            (BinOneNetSrv Lib3rdAdapter LibSprCore LibEpollEvent)
