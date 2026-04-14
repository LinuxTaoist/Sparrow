# modules_config.cmake
set(MODULE_CONFIG_VERSION "DEFAULT_MCONFIG_1002")

## 业务模块配置
# 增加变量名对应Components/Business路径下的文件夹名
# 启用方法：
#   - 修改路径： 当前文件
#   - 修改方式： BUSINESS_MODULES增加Business路径下文件夹名
list(APPEND BUSINESS_MODULES OneNetMqtt)

## 可选功能模块配置
# 非核心组件，编译时默认禁用
set(BUILD_DEBUG OFF CACHE BOOL "Build Debug")
set(BUILD_EXAMPLES OFF CACHE BOOL "Add Examples")
set(BUILD_TESTCASE OFF CACHE BOOL "Add Test Case")
