## Basics

## Project configures path
set (GENERAL_CONFIGURE_PATH ${PROJECT_PATH}/ProjectConfigs/General)
set (VENDOR_CONFIGURE_PATH  ${PROJECT_PATH}/ProjectConfigs/Vendor/${PROJECT_PLATFORM})

## Project general configures
file(COPY ${GENERAL_CONFIGURE_PATH}/default.prop        DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/init.conf           DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/OneNetDevices.conf  DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/sprlog.conf         DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${GENERAL_CONFIGURE_PATH}/system.prop         DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)

## Project vendor configures
file(COPY ${VENDOR_CONFIGURE_PATH}/vendor.prop          DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)

add_subdirectory(BinderManagerSrv)
add_subdirectory(DebugSrv)
add_subdirectory(LogManagerSrv)
add_subdirectory(MediatorSrv)
add_subdirectory(PowerManagerSrv)
add_subdirectory(PropertiesSrv)
add_subdirectory(ServiceManagerSrv)
add_subdirectory(SparrowSrv)

