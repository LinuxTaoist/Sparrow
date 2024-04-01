## Build libsprext.so
include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)
include_directories (${PROJECT_PATH}/Components/BinderSrv/Library)
aux_source_directory(${PROJECT_PATH}/ExternInterfaces        SRC_SPRCLIENT_PATH)

add_library(LibSprClient SHARED ${SRC_SPRCLIENT_PATH})
set_target_properties(LibSprClient PROPERTIES OUTPUT_NAME "sprclient")
set_target_properties(LibSprClient PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(LibSprClient stdc++ rt sprbinder dl)
add_dependencies     (LibSprClient librt libsprbinder libdl)