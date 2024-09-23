## Business
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/Core)
include_directories     (${PROJECT_PATH}/3rdAdapter)
include_directories     (${PROJECT_PATH}/UtilModules/EpollEventHandler)
include_directories     (${PROJECT_PATH}/UtilModules/MQTT)

# # Bin onenetserver
# aux_source_directory    (${PROJECT_PATH}/Util                               SRC_ONENET_SRV_PATH)
# aux_source_directory    (${PROJECT_PATH}/UtilModules/MQTT                   SRC_ONENET_SRV_PATH)
# aux_source_directory    (${PROJECT_PATH}/Components/Business/OneNetMqtt     SRC_ONENET_SRV_PATH)
# add_executable          (BinOneNetSrv ${SRC_ONENET_SRV_PATH})
# set_target_properties   (BinOneNetSrv PROPERTIES OUTPUT_NAME                "onenetsrv")
# target_link_libraries   (BinOneNetSrv 3rdadapter sprcore epollevent pthread rt)
# add_dependencies        (BinOneNetSrv Lib3rdAdapter LibSprCore LibEpollEvent)

# Lib pluginonenet
aux_source_directory    (${PROJECT_PATH}/Util                               SRC_ONENET_SRV_PATH)
aux_source_directory    (${PROJECT_PATH}/UtilModules/MQTT                   SRC_ONENET_SRV_PATH)
aux_source_directory    (${PROJECT_PATH}/Components/Business/OneNetMqtt     SRC_ONENET_SRV_PATH)
add_library             (LibOneNet SHARED ${SRC_ONENET_SRV_PATH})
set_target_properties   (LibOneNet PROPERTIES OUTPUT_NAME "pluginonenet")
set_target_properties   (LibOneNet PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries   (LibOneNet stdc++ 3rdadapter sprcore epollevent pthread rt dl)
add_dependencies        (LibOneNet Lib3rdAdapter LibSprCore LibEpollEvent)
