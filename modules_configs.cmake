# modules_configures.cmake
set(MODULE_CONFIG_VERSION "DEFAULT_MCONFIG_1001")

# 业务模块 Components/Business编译开关
set(BUSINESS_MODULES "")
list(APPEND BUSINESS_MODULES OneNetMqtt)

# 其他编译默认值, 可在Build/options中修改
set(TEST_DEBUG OFF CACHE BOOL "Add Test Debug")
set(BUILD_GTEST OFF CACHE BOOL "Add Google Test")
