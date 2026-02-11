## quectel ar590 compile options
message("load ql_ar590_compile_options.cmake")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(TOOLCHAIN_DIR "/opt/crosstools/ar59x-truck20-gcc950-v1-toolchain/x86_64-unisocsdk-linux/usr/bin/")
set(TOOLCHAIN_PREFIX "aarch64-unisoc-linux/aarch64-unisoc-linux")
set(CMAKE_C_COMPILER "${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}-g++")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-dynamic-linker,/lib/ld-linux-armhf.so.3")

## 设置链接选项
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")

# 设置 sysroot
set(CMAKE_SYSROOT "/opt/ql_sdk/AR590UACN00AAR01A048G1_OCPU_E01_V01_SDK/sysroot")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

## 可选功能模块配置
# 非核心模块，编译时默认禁用
# 启用方法：取消对应行的注释符号(#)
set(BUILD_DEBUG ON)       # 调试工具集（含调试符号与增强日志）
set(BUILD_EXAMPLES ON)    # 示例程序（演示API用法，非生产环境）
# set(BUILD_TESTCASE ON)    # 单元测试套件（依赖gtest库）

# 设置编译选项
add_compile_options(
    -Werror
    -Wall
    -O2
    -pipe
    -g2
    -fPIC
)

# 设置包含目录
include_directories(
    ${CMAKE_SYSROOT}/usr/include
    ${CMAKE_SYSROOT}/usr/include/glib-2.0
    ${CMAKE_SYSROOT}/usr/lib/glib-2.0/include
    ${CMAKE_SYSROOT}/usr/include/ql-sdk
)
