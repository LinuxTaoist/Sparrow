# Build libsprbinder.so
set(BINDHUB_PATH        ${CMAKE_CURRENT_SOURCE_DIR})
include_directories     (${BINDHUB_PATH})

# Bin binderserver
include_directories     (${PROJECT_PATH}/PrivateAPIs/Binder/)
include_directories     (${PROJECT_PATH}/3rdAdapter)
aux_source_directory    (${PROJECT_PATH}/Util       SRC_BINDER_SRV_PATH)
aux_source_directory    (${BINDHUB_PATH}            SRC_BINDER_SRV_PATH)
set(PARCEL_SRC_FILES    ${PROJECT_PATH}/PrivateAPIs/Binder/Parcel.cpp)

add_executable          (BinBinderSrv ${SRC_BINDER_SRV_PATH}    ${PARCEL_SRC_FILES})
set_target_properties   (BinBinderSrv PROPERTIES OUTPUT_NAME    "bindhubsrv")
target_link_libraries   (BinBinderSrv pthread rt 3rdadapter)
add_dependencies        (BinBinderSrv Lib3rdAdapter)
