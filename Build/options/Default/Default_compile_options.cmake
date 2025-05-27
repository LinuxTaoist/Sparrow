## default compile options
message("load default_compile_options.cmake")

# 设置链接选项
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")

# 设置编译开关
set(BUILD_DEBUG ON)
set(BUILD_EXAMPLES ON)
set(BUILD_TESTCASE ON)
