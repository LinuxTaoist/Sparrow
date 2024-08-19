## Business
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/NewCore)
include_directories     (${PROJECT_PATH}/3rdAdapter)
include_directories     (${PROJECT_PATH}/UtilModules/EpollEventHandler)

# Bin powermanagersrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/PowerManagerSrv      SRC_POWER_MANAGER_PATH)
add_executable              (BinPowerManagerSrv ${SRC_POWER_MANAGER_PATH})
set_target_properties       (BinPowerManagerSrv PROPERTIES OUTPUT_NAME              "powermanagersrv")
target_link_libraries       (BinPowerManagerSrv pthread sprnewcore epollevent 3rdadapter)
add_dependencies            (BinPowerManagerSrv LibNewSprCore Lib3rdAdapter LibEpollEvent)