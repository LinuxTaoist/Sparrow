# Bin propertiessrv
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/Core)
include_directories     (${PROJECT_PATH}/3rdAdapter)
include_directories     (${PROJECT_PATH}/PrivateAPIs/Binder)
include_directories     (${PROJECT_PATH}/UtilModules/EpollEventHandler)

aux_source_directory    (${PROJECT_PATH}/Util                                   SRC_PROPERTIES_SRV_PATH)
aux_source_directory    (${PROJECT_PATH}/Components/Basics/PropertiesSrv        SRC_PROPERTIES_SRV_PATH)
add_executable          (BinPropertySrv ${SRC_PROPERTIES_SRV_PATH})
set_target_properties   (BinPropertySrv PROPERTIES OUTPUT_NAME                  "propertiessrv")
target_link_libraries   (BinPropertySrv pthread rt sprcore sprbinder 3rdadapter epollevent)
add_dependencies        (BinPropertySrv LibSprCore LibBinder Lib3rdAdapter LibEpollEvent)