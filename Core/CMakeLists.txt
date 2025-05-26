## Build libnewsprcore.so
set(OTHER_SRC_FILES         ${PROJECT_PATH}/Util/Backtrace.cpp
                            ${PROJECT_PATH}/Util/GeneralUtils.cpp
                            ${PROJECT_PATH}/Util/Parcel.cpp
                            ${PROJECT_PATH}/Util/ProcMutex.cpp
                            ${PROJECT_PATH}/Util/RunningTiming.cpp)

aux_source_directory        (${CMAKE_CURRENT_SOURCE_DIR}        SRC_CORE_PATH)
aux_source_directory        (${PROJECT_PATH}/PrivateAPIs/Binder SRC_CORE_PATH)

add_library                 (LibSprCore SHARED  ${SRC_CORE_PATH} ${OTHER_SRC_FILES})
target_include_directories  (LibSprCore PRIVATE ${PROJECT_PATH}/Util
                                                ${PROJECT_PATH}/3rdAdapter
                                                ${CMAKE_CURRENT_SOURCE_DIR}
                                                ${PROJECT_PATH}/PrivateAPIs/Binder
                                                ${PROJECT_PATH}/UtilModules/EpollEventHandler)

set_target_properties       (LibSprCore PROPERTIES OUTPUT_NAME "sprcore")
set_target_properties       (LibSprCore PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries       (LibSprCore stdc++ sprlog epollevent rt)
add_dependencies            (LibSprCore LibSprLog LibEpollEvent)
