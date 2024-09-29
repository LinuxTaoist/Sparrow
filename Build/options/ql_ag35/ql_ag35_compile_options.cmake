## quctel ag35 compile options
message("load ql_ag35_compile_options.cmake")
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(TOOLCHAIN_DIR "/opt/ql_crosstools/ql-ag35-le22-gcc640-v1-toolchain/gcc/usr/bin/")
set(TOOLCHAIN_PREFIX "arm-oe-linux-gnueabi/arm-oe-linux-gnueabi-")
set(CMAKE_C_COMPILER "${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}g++")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-dynamic-linker,/lib/ld-linux-armhf.so.3")

# 设置 sysroot
set(CMAKE_SYSROOT "/opt/ql_sdk/ql-ol-extsdk-ag35cevnr12a02m4g_ocpu/ql-sysroots")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# 设置编译选项
add_compile_options(
    -march=armv7-a
    -marm
    -mfpu=neon
    -mfloat-abi=hard
    -Werror
    -Wall
    -finline-functions
    -finline-limit=64
    -O2
    -g2
    -fPIC
)

# 设置包含目录
include_directories(
    ${CMAKE_SYSROOT}/include
    ${CMAKE_SYSROOT}/usr/include
    ${CMAKE_SYSROOT}/usr/include/glib-2.0
    ${CMAKE_SYSROOT}/usr/lib/glib-2.0/include
    ${CMAKE_SYSROOT}/usr/include/ql_lib_utils
    ${CMAKE_SYSROOT}/usr/include/ql-sdk
)
