# PrivateAPIs
set(PRIVATE_APIS_PATH       ${CMAKE_CURRENT_SOURCE_DIR})

include_directories         (${PROJECT_PATH}/Core)
include_directories         (${PROJECT_PATH}/Util)
include_directories         (${PROJECT_PATH}/3rdAdapter)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Binder)
include_directories         (${PROJECT_PATH}/PrivateAPIs/Event)
include_directories         (${PROJECT_PATH}/Components/Basics/BindHubSrv)
aux_source_directory        (${PROJECT_PATH}/Util           SRC_BINDER_LIB_PATH)
aux_source_directory        (${PRIVATE_APIS_PATH}/Binder    SRC_BINDER_LIB_PATH)
aux_source_directory        (${PRIVATE_APIS_PATH}/Event     SRC_BINDER_LIB_PATH)

## Build libsprbinder.so
add_library(LibBinder       SHARED  ${SRC_BINDER_LIB_PATH})
set_target_properties       (LibBinder PROPERTIES OUTPUT_NAME "sprbinder")
set_target_properties       (LibBinder PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibBinder stdc++ pthread rt dl LibSprLog)
add_dependencies            (LibBinder LibSprLog)

## Build libmediatorapi.so
set(SRC_MEDIATOR_API_FILES  ${PRIVATE_APIS_PATH}/SprMediatorInterface.cpp)
add_library                 (LibMediatorAPI SHARED ${SRC_MEDIATOR_API_FILES})
set_target_properties       (LibMediatorAPI PROPERTIES OUTPUT_NAME "mediatorapi")
set_target_properties       (LibMediatorAPI PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibMediatorAPI stdc++ rt dl LibBinder)
add_dependencies            (LibMediatorAPI LibBinder)

## Build libdebugapi.so
set(SRC_DEBUG_API_FILES     ${PRIVATE_APIS_PATH}/DebugInterface.cpp)
add_library                 (LibDebugAPI SHARED ${SRC_DEBUG_API_FILES})
set_target_properties       (LibDebugAPI PROPERTIES OUTPUT_NAME "debugapi")
set_target_properties       (LibDebugAPI PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibDebugAPI stdc++ rt dl LibBinder)
add_dependencies            (LibDebugAPI LibBinder)

## Build libsprmain.so
set(SRC_SPR_MAIN_FILES      ${PROJECT_PATH}/Util/Backtrace.cpp
                            ${PROJECT_PATH}/Util/GeneralUtils.cpp
                            ${PRIVATE_APIS_PATH}/SprMainInterface.cpp)
add_library                 (LibSprMain SHARED  ${SRC_SPR_MAIN_FILES})
target_include_directories  (LibSprMain PRIVATE ${PROJECT_PATH}/3rdAdapter
                                                ${PROJECT_PATH}/PrivateAPIs/Event
                                                ${PROJECT_PATH}/UtilModules/EpollEventHandler)
set_target_properties       (LibSprMain PROPERTIES OUTPUT_NAME "sprmain")
set_target_properties       (LibSprMain PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibSprMain stdc++ LibSprCore rt dl)
add_dependencies            (LibSprMain LibSprCore)
