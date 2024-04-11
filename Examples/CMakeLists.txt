## Examples

include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)
include_directories (${PROJECT_PATH}/3rdAdapter)

## Bin debugcore
set(SRC_BIN_DCORE           DebugSprCore.cc)
add_executable              (debug_core ${SRC_BIN_DCORE})
set_target_properties       (debug_core PROPERTIES OUTPUT_NAME "debugcore")
target_link_libraries       (debug_core c pthread sprcore)
add_dependencies            (debug_core LibSprCore)

## Bin debugmsg
set(SRC_BIN_DMSG            DebugMsg.cc)
add_executable              (debug_msg ${SRC_BIN_DMSG})
set_target_properties       (debug_msg PROPERTIES OUTPUT_NAME "debugmsg")
target_link_libraries       (debug_msg c 3rdadapter sprcore)
add_dependencies            (debug_msg Lib3rdAdapter LibSprCore)

## Bin debugbinder
set(SRC_BIN_DBINDER         DebugBinder.cc)
add_executable              (debug_binder ${SRC_BIN_DBINDER})
target_include_directories  (debug_binder PRIVATE ${PROJECT_PATH}/Components/BinderSrv/Library)
set_target_properties       (debug_binder PROPERTIES OUTPUT_NAME "debugbinder")
target_link_libraries       (debug_binder c sprbinder)
add_dependencies            (debug_binder LibBinder)

## Bin property_get
set(SRC_BIN_PROPERTY_GET     GetProperty.cc)
add_executable              (property_get ${SRC_BIN_PROPERTY_GET})
target_include_directories  (property_get PRIVATE ${PROJECT_PATH}/PublicAPIs)
set_target_properties       (property_get PROPERTIES OUTPUT_NAME "property_get")
target_link_libraries       (property_get c propertyapi)
add_dependencies            (property_get LibPropertyAPI)

## Bin property_set
set(SRC_BIN_PROPERTY_SET     SetProperty.cc)
add_executable              (property_set ${SRC_BIN_PROPERTY_SET})
target_include_directories  (property_set PRIVATE ${PROJECT_PATH}/PublicAPIs)
set_target_properties       (property_set PROPERTIES OUTPUT_NAME "property_set")
target_link_libraries       (property_set c propertyapi)
add_dependencies            (property_set LibPropertyAPI)
