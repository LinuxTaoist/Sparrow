# Bin logservice
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/NewCore)
include_directories     (${PROJECT_PATH}/3rdAdapter)

aux_source_directory        (${PROJECT_PATH}/Components/Basics/LogManagerSrv        SRC_LOG_SRV_PATH)
add_executable              (BinLogSrv ${SRC_LOG_SRV_PATH})
set_target_properties       (BinLogSrv PROPERTIES OUTPUT_NAME                       "logmanagersrv")
target_link_libraries       (BinLogSrv pthread sprcore)
add_dependencies            (BinLogSrv LibSprCore)