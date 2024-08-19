## Business
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/NewCore)
include_directories     (${PROJECT_PATH}/3rdAdapter)
include_directories     (${PROJECT_PATH}/PrivateAPIs/Binder)

# Bin binderserver
aux_source_directory    (${PROJECT_PATH}/Util                                   SRC_BINDER_SRV_PATH)
aux_source_directory    (${PROJECT_PATH}/Components/Basics/BinderManagerSrv     SRC_BINDER_SRV_PATH)
add_executable          (BinBinderSrv ${SRC_BINDER_SRV_PATH})
set_target_properties   (BinBinderSrv PROPERTIES OUTPUT_NAME                    "bindermanagersrv")
target_link_libraries   (BinBinderSrv pthread rt 3rdadapter)
add_dependencies        (BinBinderSrv Lib3rdAdapter)