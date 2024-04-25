## PublicAPIs

include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)
include_directories (${PROJECT_PATH}/Components/BinderSrv/Library)

## Build libmediator.so
set (SRC_MEDIATOR_API_PATH  ${PROJECT_PATH}/PrivateAPIs/SprMediatorInterface.cpp)
add_library          (LibMediatorAPI SHARED ${SRC_MEDIATOR_API_PATH})
set_target_properties(LibMediatorAPI PROPERTIES OUTPUT_NAME "mediatorapi")
set_target_properties(LibMediatorAPI PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(LibMediatorAPI stdc++ rt dl sprbinder)
add_dependencies     (LibMediatorAPI LibBinder)