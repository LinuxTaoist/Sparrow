# Build libsprbinder.so
set(BINDER_PATH ${PROJECT_PATH}/Components/BinderSrv/)
include_directories (${BINDER_PATH})
aux_source_directory(${PROJECT_PATH}/Util   SRC_BINDER_LIB_PATH)
aux_source_directory(${BINDER_PATH}/Library SRC_BINDER_LIB_PATH)

add_library(LibBinder SHARED ${SRC_BINDER_LIB_PATH})
set_target_properties(LibBinder PROPERTIES OUTPUT_NAME "sprbinder")
set_target_properties(LibBinder PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(LibBinder stdc++ rt dl)

# Bin binderserver
include_directories (${BINDER_PATH}/Library)
aux_source_directory(${PROJECT_PATH}/Util                   SRC_BINDER_SRV_PATH)
aux_source_directory(${PROJECT_PATH}/Components/BinderSrv   SRC_BINDER_SRV_PATH)
set(PARCEL_FILES    ${BINDER_PATH}/Library/Parcel.cpp)

add_executable       (BinBinderSrv ${SRC_BINDER_SRV_PATH}   ${PARCEL_FILES})
set_target_properties(BinBinderSrv PROPERTIES OUTPUT_NAME   "bindersrv")
target_link_libraries(BinBinderSrv pthread rt)
