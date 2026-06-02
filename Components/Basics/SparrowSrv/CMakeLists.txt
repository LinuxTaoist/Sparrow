## SparrowSrv

# TimeManager source
set(TIME_MANAGER_PATH       ${PROJECT_PATH}/Components/Basics/TimeManager)
set(TIME_MANAGER_SOURCES    ${TIME_MANAGER_PATH}/TimeManager.cpp
                            ${TIME_MANAGER_PATH}/NtpProtocol.cpp
                            ${TIME_MANAGER_PATH}/NtpSource.cpp)

# Bin sparrowsrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/SparrowSrv   SRC_SPARROW_SRV_PATH)
add_executable              (BinMainSpr ${SRC_SPARROW_SRV_PATH} ${TIME_MANAGER_SOURCES})
target_include_directories  (BinMainSpr PRIVATE ${TIME_MANAGER_PATH}
                                                ${PROJECT_PATH}/Util
                                                ${PROJECT_PATH}/Core
                                                ${PROJECT_PATH}/3rdAdapter
                                                ${PROJECT_PATH}/UtilModules/EpollEventHandler)

set_target_properties       (BinMainSpr PROPERTIES OUTPUT_NAME "sparrowsrv")
target_link_libraries       (BinMainSpr sprcore epollevent dl rt sprlog pthread)
add_dependencies            (BinMainSpr LibSprCore LibEpollEvent LibSprLog)
