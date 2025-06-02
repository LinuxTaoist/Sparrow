# modules_configures.cmake
set(MODULE_CONFIG_VERSION "DEFAULT_MCONFIG_1001")

## 业务模块配置
# 增加变量名对应Components/Business路径下的文件夹名
# 启用方法：
#   - 修改路径： 当前文件
#   - 修改方式： BUSINESS_MODULES增加对应文件夹名
set(BUSINESS_MODULES "")
list(APPEND BUSINESS_MODULES OneNetMqtt)

## 可选功能模块配置
# 非核心组件，编译时默认禁用
# 启用方法：参考 Build/options/{platform}_compile_options.cmake
set(BUILD_DEBUG OFF CACHE BOOL "Build Debug")
set(BUILD_EXAMPLES OFF CACHE BOOL "Add Examples")
set(BUILD_TESTCASE OFF CACHE BOOL "Add Test Case")
