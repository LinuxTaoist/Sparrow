# Hardware/Default/CMakeLists.txt —— Default (stub) HAL implementation
#
# This is the fallback when no HAL_FAMILY is declared.
# All methods return HW_ERR_NOT_IMPL.
# Real platforms replace this with Hardware/<Family>/.

add_library(LibSprHal SHARED
    ../Common/src/HwInterfaceHub.cpp
    src/HwPowerDefault.cpp
    src/HwGpioDefault.cpp
    src/HwUartDefault.cpp
    src/HwWatchdogDefault.cpp
    src/HwRtcDefault.cpp
    src/HwNetworkDefault.cpp
    src/HwRegisterDefault.cpp
)

target_include_directories(LibSprHal PRIVATE    ${CMAKE_CURRENT_SOURCE_DIR}/inc
                                                ${CMAKE_CURRENT_SOURCE_DIR}/../Common/inc)
set_target_properties(LibSprHal PROPERTIES OUTPUT_NAME "sprhal")
set_target_properties(LibSprHal PROPERTIES LINKER_LANGUAGE CXX)
