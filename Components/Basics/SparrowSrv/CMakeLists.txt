# Bin sparrowsrv
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/NewCore)
include_directories     (${PROJECT_PATH}/3rdAdapter)
include_directories     (${PROJECT_PATH}/UtilModules/EpollEventHandler)

aux_source_directory    (${PROJECT_PATH}/Components/Basics/SparrowSrv   SRC_SPARROW_SRV_PATH)
add_executable          (BinMainSpr ${SRC_SPARROW_SRV_PATH})
set_target_properties   (BinMainSpr PROPERTIES OUTPUT_NAME              "sparrowsrv")
target_link_libraries   (BinMainSpr sprnewcore epollevent)
add_dependencies        (BinMainSpr LibSprCore LibEpollEvent)
