## OneNetMqtt

# add source files
aux_source_directory    (${PROJECT_PATH}/UtilModules/MQTT                   SRC_ONENET_SRV_PATH)
aux_source_directory    (${PROJECT_PATH}/Components/Business/OneNetMqtt     SRC_ONENET_SRV_PATH)
set(OTHER_SRC_FILES     ${PROJECT_PATH}/Util/cJSON.c)

# Lib pluginonenet
add_library                 (LibOneNet SHARED   ${SRC_ONENET_SRV_PATH}
                                                ${OTHER_SRC_FILES})
target_include_directories  (LibOneNet PRIVATE  ${PROJECT_PATH}/Util
                                                ${PROJECT_PATH}/Core
                                                ${PROJECT_PATH}/3rdAdapter
                                                ${PROJECT_PATH}/UtilModules/EpollEventHandler
                                                ${PROJECT_PATH}/UtilModules/MQTT)

set_target_properties       (LibOneNet PROPERTIES OUTPUT_NAME "pluginonenet")
set_target_properties       (LibOneNet PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibOneNet stdc++ sprlog sprcore epollevent pthread rt dl sprlog)
add_dependencies            (LibOneNet LibSprLog LibSprCore LibEpollEvent LibSprLog)