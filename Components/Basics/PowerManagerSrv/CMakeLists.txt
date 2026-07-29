## PowerManagerSrv

# Bin powermanagersrv
aux_source_directory        (${PROJECT_PATH}/PrivateAPIs/Event                      SRC_POWER_MANAGER_PATH)
aux_source_directory        (${PROJECT_PATH}/Components/Basics/PowerManagerSrv      SRC_POWER_MANAGER_PATH)
add_executable              (BinPowerManagerSrv ${SRC_POWER_MANAGER_PATH})
set_target_properties       (BinPowerManagerSrv PROPERTIES OUTPUT_NAME              "powermanagersrv")
target_include_directories  (BinPowerManagerSrv PRIVATE ${PROJECT_PATH}/Util
                                                        ${PROJECT_PATH}/Core
                                                        ${PROJECT_PATH}/3rdAdapter
                                                        ${PROJECT_PATH}/PrivateAPIs/Event
                                                        ${PROJECT_PATH}/UtilModules/EpollEventHandler)
target_link_libraries       (BinPowerManagerSrv pthread LibSprCore)
add_dependencies            (BinPowerManagerSrv LibSprCore)