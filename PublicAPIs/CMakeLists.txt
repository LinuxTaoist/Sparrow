## PublicAPIs

include_directories (${PROJECT_PATH}/Core)
include_directories (${PROJECT_PATH}/Util)
include_directories (${PROJECT_PATH}/PrivateAPIs/Binder)

## Build libpropertyapi.so
set (SRC_PROPERTY_API_PATH  ${PROJECT_PATH}/PublicAPIs/Property.cpp)
add_library          (LibPropertyAPI SHARED ${SRC_PROPERTY_API_PATH})
set_target_properties(LibPropertyAPI PROPERTIES OUTPUT_NAME "propertyapi")
set_target_properties(LibPropertyAPI PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(LibPropertyAPI stdc++ rt dl sprbinder)
add_dependencies     (LibPropertyAPI LibBinder)
