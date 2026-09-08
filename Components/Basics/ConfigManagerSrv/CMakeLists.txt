# ConfigManagerSrv

## Bin configmanagersrv
aux_source_directory        (${PROJECT_PATH}/Components/Basics/ConfigManagerSrv        SRC_CONFIG_SRV_PATH)
add_executable              (BinConfigManagerSrv ${SRC_CONFIG_SRV_PATH})
target_include_directories  (BinConfigManagerSrv PRIVATE ${PROJECT_PATH}/Util
                                                         ${PROJECT_PATH}/Core
                                                         ${PROJECT_PATH}/3rdAdapter
                                                         ${PROJECT_PATH}/PrivateAPIs/Binder
                                                         ${PROJECT_PATH}/3rdParty/sqlite/include)
set_target_properties       (BinConfigManagerSrv PROPERTIES OUTPUT_NAME "configmanagersrv")
target_link_libraries       (BinConfigManagerSrv pthread rt LibSprCore LibBinder LibSqliteAdapter dl)
add_dependencies            (BinConfigManagerSrv LibSprCore LibBinder LibSqliteAdapter)