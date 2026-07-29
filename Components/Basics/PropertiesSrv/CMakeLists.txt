## PropertiesSrv

# Bin propertiessrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/PropertiesSrv        SRC_PROPERTIES_SRV_PATH)
add_executable              (BinPropertySrv ${SRC_PROPERTIES_SRV_PATH})
target_include_directories  (BinPropertySrv PRIVATE ${PROJECT_PATH}/Util
                                                    ${PROJECT_PATH}/Core
                                                    ${PROJECT_PATH}/3rdAdapter
                                                    ${PROJECT_PATH}/PrivateAPIs/Binder
                                                    ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (BinPropertySrv PROPERTIES OUTPUT_NAME "propertiessrv")
target_link_libraries       (BinPropertySrv pthread rt LibSprCore LibBinder LibSprLog dl)
add_dependencies            (BinPropertySrv LibSprCore LibBinder LibSprLog)