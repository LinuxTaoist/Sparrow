# SparrowSrv

## Bin sparrowsrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/SparrowSrv   SRC_SPARROW_SRV_PATH)
add_executable              (BinMainSpr ${SRC_SPARROW_SRV_PATH})
target_include_directories  (BinMainSpr PRIVATE ${PROJECT_PATH}/Util
                                                ${PROJECT_PATH}/Core
                                                ${PROJECT_PATH}/3rdAdapter
                                                ${PROJECT_PATH}/PrivateAPIs
                                                ${PROJECT_PATH}/UtilModules/EpollEventHandler)

set_target_properties       (BinMainSpr PROPERTIES OUTPUT_NAME "sparrowsrv")
target_link_libraries       (BinMainSpr LibSprCore LibSprMain dl rt pthread)
add_dependencies            (BinMainSpr LibSprCore LibSprMain)
