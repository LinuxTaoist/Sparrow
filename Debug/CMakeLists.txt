## Debug
set(DEBUG_PATH ${PROJECT_PATH}/Debug)

include_directories (${PROJECT_PATH}/Util)

# Bin logshow
set(SRC_BIN_LOGSHOW         ${DEBUG_PATH}/LogShow/main_logshow.cpp)
aux_source_directory        (${PROJECT_PATH}/Util SRC_OTHER_PATH)
add_executable              (BinLogShow ${SRC_BIN_LOGSHOW} ${SRC_OTHER_PATH})
set_target_properties       (BinLogShow PROPERTIES OUTPUT_NAME "logshow")
target_link_libraries       (BinLogShow pthread)

# Bin infrawatch
aux_source_directory(${PROJECT_PATH}/Debug/InfraWatch SRC_INFRA_WATCH_PATH)
add_executable              (BinInfraWatch ${SRC_INFRA_WATCH_PATH})
set_target_properties       (BinInfraWatch PROPERTIES OUTPUT_NAME "infrawatch")
target_link_libraries       (BinInfraWatch rt pthread)
# add_dependencies            (BinInfraWatch LibSprCore)
