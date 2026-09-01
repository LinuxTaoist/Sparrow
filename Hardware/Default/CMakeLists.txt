# Hardware/Default

## Build libsprhal.so
set(DEFAULT_HAL_PATH        ${PROJECT_PATH}/Hardware/Default)
set(DEFAULT_HAL_SRC_FILES   ${DEFAULT_HAL_PATH}/../Common/src/HwInterfaceHub.cpp
                            ${DEFAULT_HAL_PATH}/src/HwPowerDefault.cpp
                            ${DEFAULT_HAL_PATH}/src/HwGpioDefault.cpp
                            ${DEFAULT_HAL_PATH}/src/HwUartDefault.cpp
                            ${DEFAULT_HAL_PATH}/src/HwWatchdogDefault.cpp
                            ${DEFAULT_HAL_PATH}/src/HwRtcDefault.cpp
                            ${DEFAULT_HAL_PATH}/src/HwNetworkDefault.cpp
                            ${DEFAULT_HAL_PATH}/src/HwDeviceDefault.cpp
                            ${DEFAULT_HAL_PATH}/src/HwSimDefault.cpp
                            ${DEFAULT_HAL_PATH}/src/HwRegisterDefault.cpp)

add_library                 (LibSprHal SHARED   ${DEFAULT_HAL_SRC_FILES})
target_include_directories  (LibSprHal PRIVATE  ${DEFAULT_HAL_PATH}/inc
                                                ${DEFAULT_HAL_PATH}/../Common/inc)
set_target_properties       (LibSprHal PROPERTIES OUTPUT_NAME "sprhal")
set_target_properties       (LibSprHal PROPERTIES LINKER_LANGUAGE CXX)
