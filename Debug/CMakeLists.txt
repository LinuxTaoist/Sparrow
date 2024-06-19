## Debug
set(SRC_DEBUG_PATH          ${PROJECT_PATH}/Debug)

include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/Util)
include_directories         (${PROJECT_PATH}/PrivateAPIs)
include_directories         (${PROJECT_PATH}/PublicAPIs)

# Bin logshow
aux_source_directory        (${PROJECT_PATH}/Util               SRC_BIN_LOGSHOW)
aux_source_directory        (${SRC_DEBUG_PATH}/LogShow          SRC_BIN_LOGSHOW)
add_executable              (BinLogShow ${SRC_BIN_LOGSHOW})
set_target_properties       (BinLogShow PROPERTIES OUTPUT_NAME  "logshow")
target_link_libraries       (BinLogShow pthread)

# Bin infrawatch
aux_source_directory        (${PROJECT_PATH}/Util               SRC_INFRA_WATCH_PATH)
aux_source_directory        (${SRC_DEBUG_PATH}/InfraWatch       SRC_INFRA_WATCH_PATH)
add_executable              (BinInfraWatch ${SRC_INFRA_WATCH_PATH})
set_target_properties       (BinInfraWatch PROPERTIES OUTPUT_NAME "infrawatch")
target_link_libraries       (BinInfraWatch sprcore mediatorapi powermanagerapi debugapi rt pthread)
add_dependencies            (BinInfraWatch LibSprCore LibMediatorAPI LibPowerManagerAPI LibDebugAPI)

# Bin rshellx
aux_source_directory        (${PROJECT_PATH}/Util               SRC_RSHELLX_PATH)
aux_source_directory        (${SRC_DEBUG_PATH}/RShellX          SRC_RSHELLX_PATH)
add_executable              (BinRShellX ${SRC_RSHELLX_PATH})
target_include_directories  (BinRShellX PRIVATE ${PROJECT_PATH}/Core/EpollEventHandler)
set_target_properties       (BinRShellX PROPERTIES OUTPUT_NAME  "rshellx")
target_link_libraries       (BinRShellX c epollevent pthread)
add_dependencies            (BinRShellX LibEpollEvent)
