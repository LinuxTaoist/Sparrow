# modules_config.cmake
set(MODULE_CONFIG_VERSION "DEFAULT_MCONFIG_1002")

## 业务模块配置
# 增加变量名对应Components/Business路径下的文件夹名
# 启用方法：
#   - 修改路径： 当前文件
#   - 修改方式： BUSINESS_MODULES增加Business路径下文件夹名
list(APPEND BUSINESS_MODULES OneNetMqtt)

## 可选功能模块配置
# 非核心模块，编译时默认禁用
# 启用方法：取消对应行的注释符号(#)
set(BUILD_DEBUG ON)       # 调试工具集（含调试符号与增强日志）
set(BUILD_EXAMPLES ON)    # 示例程序（演示API用法，非生产环境）
set(BUILD_TESTCASE ON)    # 单元测试套件（依赖gtest库）
