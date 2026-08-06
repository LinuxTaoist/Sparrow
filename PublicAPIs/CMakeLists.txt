# PublicAPIs

include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/Util)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Binder)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Event)

## Build libpowermanagerapi.so
set (SRC_POWERM_API_PATH    ${PROJECT_PATH}/PublicAPIs/PowerManagerInterface.cpp)
add_library                 (LibPowerManagerAPI SHARED ${SRC_POWERM_API_PATH})
set_target_properties       (LibPowerManagerAPI PROPERTIES OUTPUT_NAME "powermanagerapi")
set_target_properties       (LibPowerManagerAPI PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibPowerManagerAPI stdc++ rt dl LibBinder)
add_dependencies            (LibPowerManagerAPI LibBinder)

## Build libpropertyapi.so
set (SRC_PROPERTY_API_PATH  ${PROJECT_PATH}/PublicAPIs/Property.cpp)
add_library                 (LibPropertyAPI SHARED ${SRC_PROPERTY_API_PATH})
set_target_properties       (LibPropertyAPI PROPERTIES OUTPUT_NAME "propertyapi")
set_target_properties       (LibPropertyAPI PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibPropertyAPI stdc++ rt dl LibBinder)
add_dependencies            (LibPropertyAPI LibBinder)

## Build libconfigapi.so
set (SRC_CONFIG_API_PATH    ${PROJECT_PATH}/PublicAPIs/Config.cpp)
add_library                 (LibConfigAPI SHARED ${SRC_CONFIG_API_PATH})
set_target_properties       (LibConfigAPI PROPERTIES OUTPUT_NAME "configapi")
set_target_properties       (LibConfigAPI PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibConfigAPI stdc++ rt dl LibBinder)
add_dependencies            (LibConfigAPI LibBinder)
