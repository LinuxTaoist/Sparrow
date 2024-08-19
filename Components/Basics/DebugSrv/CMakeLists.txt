## Business
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/NewCore)
include_directories     (${PROJECT_PATH}/3rdAdapter)
include_directories     (${PROJECT_PATH}/UtilModules/EpollEventHandler)

# Bin debugsrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/DebugSrv             DEBUG_SRV_PATH)
aux_source_directory        (${PROJECT_PATH}/Core/EpollEventHandler                 DEBUG_SRV_PATH)
add_executable              (BinDebugSrv ${DEBUG_SRV_PATH})
set_target_properties       (BinDebugSrv PROPERTIES OUTPUT_NAME                     "debugsrv")
target_link_libraries       (BinDebugSrv sprnewcore sprbinder epollevent pthread)
add_dependencies            (BinDebugSrv LibNewSprCore LibEpollEvent LibBinder)
