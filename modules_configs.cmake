# modules_configures.cmake
set(MODULE_CONFIG_VERSION "DEFAULT_MCONFIG_1001")

## 业务模块配置
# 业务组件，根据需要增加BUSINESS_MODULES中的模块
# 对应Components/Business路径下的文件夹名
set(BUSINESS_MODULES "")
list(APPEND BUSINESS_MODULES OneNetMqtt)

## 可选功能模块配置
# 非核心组件，编译时默认禁用
# 启用方法：Build/options中修改对应编译项
set(BUILD_DEBUG OFF CACHE BOOL "Build Debug")
set(BUILD_EXAMPLES OFF CACHE BOOL "Add Examples")
set(BUILD_TESTCASE OFF CACHE BOOL "Add Test Case")
