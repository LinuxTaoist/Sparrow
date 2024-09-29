## baiwen imx6ull compile options
message("load bw_imx6ull_compile_options.cmake")
set(TOOLCHAIN_DIR "/opt/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/")
set(TOOLCHAIN_PREFIX "arm-linux-gnueabihf-")
set(CMAKE_C_COMPILER "${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_DIR}${TOOLCHAIN_PREFIX}g++")
