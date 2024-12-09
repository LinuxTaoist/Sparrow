# Bin sparrowsrv
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/Core)
include_directories     (${PROJECT_PATH}/3rdAdapter)
include_directories     (${PROJECT_PATH}/UtilModules/EpollEventHandler)

# TimeManager source
set(TIME_MANAGER_PATH ${PROJECT_PATH}/Components/Basics/TimeManager)
set(TIME_MANAGER_SOURCES ${TIME_MANAGER_PATH}/TimeManager.cpp
                         ${TIME_MANAGER_PATH}/NtpClient.cpp)

aux_source_directory        (${PROJECT_PATH}/Components/Basics/SparrowSrv   SRC_SPARROW_SRV_PATH)
add_executable              (BinMainSpr ${SRC_SPARROW_SRV_PATH} ${TIME_MANAGER_SOURCES})
target_include_directories  (BinMainSpr PRIVATE ${TIME_MANAGER_PATH})
set_target_properties       (BinMainSpr PROPERTIES OUTPUT_NAME "sparrowsrv")
target_link_libraries       (BinMainSpr sprcore epollevent dl)
add_dependencies            (BinMainSpr LibSprCore LibEpollEvent)
