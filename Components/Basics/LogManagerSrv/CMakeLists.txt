## LogManagerSrv

set(OTHER_SRC_FILES     ${PROJECT_PATH}/Util/SharedRingBuffer.cpp)
aux_source_directory    (${PROJECT_PATH}/Components/Basics/LogManagerSrv    SRC_LOG_SRV_PATH)

# Bin logmanagersrv
add_executable  (BinLogSrv  ${SRC_LOG_SRV_PATH}
                            ${OTHER_SRC_FILES})
target_include_directories  (BinLogSrv PRIVATE  ${PROJECT_PATH}/Util
                                                ${PROJECT_PATH}/Core
                                                ${PROJECT_PATH}/3rdAdapter)
set_target_properties       (BinLogSrv PROPERTIES OUTPUT_NAME "logmanagersrv")
target_link_libraries       (BinLogSrv pthread sprcore)
add_dependencies            (BinLogSrv LibSprCore)
