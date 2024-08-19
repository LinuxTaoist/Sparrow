# Bin propertiessrv
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/NewCore)
include_directories     (${PROJECT_PATH}/3rdAdapter)
include_directories     (${PROJECT_PATH}/PrivateAPIs/Binder)

aux_source_directory    (${PROJECT_PATH}/Util                                   SRC_PROPERTIES_SRV_PATH)
aux_source_directory    (${PROJECT_PATH}/Components/Basics/PropertiesSrv        SRC_PROPERTIES_SRV_PATH)
add_executable          (BinPropertySrv ${SRC_PROPERTIES_SRV_PATH})
set_target_properties   (BinPropertySrv PROPERTIES OUTPUT_NAME                  "propertiessrv")
target_link_libraries   (BinPropertySrv pthread rt sprnewcore sprbinder 3rdadapter)
add_dependencies        (BinPropertySrv LibSprCore LibBinder Lib3rdAdapter)