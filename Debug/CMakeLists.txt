## Debug
set(SRC_DEBUG_PATH          ${PROJECT_PATH}/Debug)

include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/Util)
include_directories         (${PROJECT_PATH}/3rdAdapter)
include_directories         (${PROJECT_PATH}/PrivateAPIs)
include_directories         (${PROJECT_PATH}/PublicAPIs)
include_directories         (${PROJECT_PATH}/UtilModules/EpollEventHandler)

# Bin logshow
aux_source_directory        (${PROJECT_PATH}/Util               SRC_BIN_LOGSHOW)
aux_source_directory        (${SRC_DEBUG_PATH}/LogShow          SRC_BIN_LOGSHOW)
add_executable              (BinLogShow ${SRC_BIN_LOGSHOW})
set_target_properties       (BinLogShow PROPERTIES OUTPUT_NAME  "logshow")
target_link_libraries       (BinLogShow pthread rt c dl)

# Bin infrawatch
aux_source_directory        (${PROJECT_PATH}/Util               SRC_INFRA_WATCH_PATH)
aux_source_directory        (${SRC_DEBUG_PATH}/InfraWatch       SRC_INFRA_WATCH_PATH)
add_executable              (BinInfraWatch ${SRC_INFRA_WATCH_PATH})
set_target_properties       (BinInfraWatch PROPERTIES OUTPUT_NAME "infrawatch")
target_link_libraries       (BinInfraWatch sprcore mediatorapi powermanagerapi onenetapi debugapi rt pthread dl)
add_dependencies            (BinInfraWatch LibSprCore LibMediatorAPI LibPowerManagerAPI LibOneNetAPI LibDebugAPI)

## Bin getprop
set(SRC_BIN_PROPERTY_GET    ${SRC_DEBUG_PATH}/Property/GetProperty.cpp)
add_executable              (property_get ${SRC_BIN_PROPERTY_GET})
target_include_directories  (property_get PRIVATE ${PROJECT_PATH}/PublicAPIs)
set_target_properties       (property_get PROPERTIES OUTPUT_NAME "getprop")
target_link_libraries       (property_get c propertyapi)
add_dependencies            (property_get LibPropertyAPI)

## Bin setprop
set(SRC_BIN_PROPERTY_SET    ${SRC_DEBUG_PATH}/Property/SetProperty.cpp)
add_executable              (property_set ${SRC_BIN_PROPERTY_SET})
target_include_directories  (property_set PRIVATE ${PROJECT_PATH}/PublicAPIs)
set_target_properties       (property_set PROPERTIES OUTPUT_NAME "setprop")
target_link_libraries       (property_set c propertyapi)
add_dependencies            (property_set LibPropertyAPI)

# Bin rshellx
aux_source_directory        (${PROJECT_PATH}/Util               SRC_RSHELLX_PATH)
aux_source_directory        (${SRC_DEBUG_PATH}/RShellX          SRC_RSHELLX_PATH)
add_executable              (BinRShellX ${SRC_RSHELLX_PATH})
set_target_properties       (BinRShellX PROPERTIES OUTPUT_NAME  "rshellx")
target_link_libraries       (BinRShellX c epollevent sprlog rt pthread dl util)
add_dependencies            (BinRShellX LibEpollEvent LibSprLog)

# Bin propdump
set(UTIL_SRC_FILES          ${PROJECT_PATH}/Util/FileUtils.cpp)
aux_source_directory        (${SRC_DEBUG_PATH}/ProtoDump        SRC_PROTO_DUMP_PATH)
aux_source_directory        (${PROJECT_PATH}/UtilModules/CodecX SRC_PROTO_DUMP_PATH)
add_executable              (BinPropDump ${SRC_PROTO_DUMP_PATH} ${UTIL_SRC_FILES})
target_include_directories  (BinPropDump PRIVATE ${PROJECT_PATH}/UtilModules/CodecX)
set_target_properties       (BinPropDump PROPERTIES OUTPUT_NAME "propdump")
