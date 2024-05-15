## PrivateAPIs
set(PRIVATE_APIS_PATH   ${CMAKE_CURRENT_SOURCE_DIR})

include_directories     (${PROJECT_PATH}/Core)
include_directories     (${PROJECT_PATH}/Util)
include_directories     (${PROJECT_PATH}/PrivateAPIs/Binder)

## Build libsprbinder.so
include_directories     (${PROJECT_PATH}/Components/Basics/BindHubSrv)
aux_source_directory    (${PROJECT_PATH}/Util           SRC_BINDER_LIB_PATH)
aux_source_directory    (${PRIVATE_APIS_PATH}/Binder    SRC_BINDER_LIB_PATH)

add_library(LibBinder   SHARED  ${SRC_BINDER_LIB_PATH})
set_target_properties   (LibBinder PROPERTIES OUTPUT_NAME "sprbinder")
set_target_properties   (LibBinder PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries   (LibBinder stdc++ pthread rt dl)

## Build libmediator.so
set (SRC_MEDIATOR_API_PATH  ${PRIVATE_APIS_PATH}/SprMediatorInterface.cpp)
add_library             (LibMediatorAPI SHARED ${SRC_MEDIATOR_API_PATH})
set_target_properties   (LibMediatorAPI PROPERTIES OUTPUT_NAME "mediatorapi")
set_target_properties   (LibMediatorAPI PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries   (LibMediatorAPI stdc++ rt dl sprbinder)
add_dependencies        (LibMediatorAPI LibBinder)
