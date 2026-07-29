## BinderManagerSrv

set(OTHER_SRC_FILES     ${PROJECT_PATH}/Util/Backtrace.cpp
                        ${PROJECT_PATH}/Util/GeneralUtils.cpp
                        ${PROJECT_PATH}/Util/Parcel.cpp)

# Bin binderserver
aux_source_directory    (${PROJECT_PATH}/Components/Basics/BinderManagerSrv SRC_BINDER_SRV_PATH)
add_executable          (BinBinderSrv               ${SRC_BINDER_SRV_PATH}
                                                    ${OTHER_SRC_FILES})
target_include_directories (BinBinderSrv PRIVATE    ${PROJECT_PATH}/Util
                                                    ${PROJECT_PATH}/Core
                                                    ${PROJECT_PATH}/3rdAdapter
                                                    ${PROJECT_PATH}/PrivateAPIs/Binder)

set_target_properties   (BinBinderSrv PROPERTIES OUTPUT_NAME "bindermanagersrv")
target_link_libraries   (BinBinderSrv pthread LibSprLog rt dl)
add_dependencies        (BinBinderSrv LibSprLog)