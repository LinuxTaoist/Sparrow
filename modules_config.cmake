# modules_config.cmake

## 基础模块配置
# 增加变量名对应Components/Basics路径下的文件夹名
# 启用方法：
#   - 修改路径： Platform/{Platform}/Build/Options/{Platform}_compile_options.cmake
#   - 修改方式： BASIC_MODULES增加对应文件夹名
#               BUSINESS_MODULES增加对应文件夹名
set(BASIC_MODULES "")
set(BUSINESS_MODULES "")

# Hardware 平台配置
# 启用方法：
#   - 修改路径： Platform/{Platform}/Build/Options/{Platform}_compile_options.cmake
#   - 修改方式： 设置HAL_FAMILY为 Platform/Hardware 路径下对应的文件夹名
set(HAL_FAMILY "Default")

## 可选功能模块配置
# 非核心组件，编译时默认禁用
# 启用方法：参考Platform/{Platform}/Build/Options/{Platform}_compile_options.cmake
set(BUILD_DEBUG OFF CACHE BOOL "Build Debug")
set(BUILD_EXAMPLES OFF CACHE BOOL "Add Examples")
set(BUILD_TESTCASE OFF CACHE BOOL "Add Test Case")
set(BUILD_COVERAGE OFF CACHE BOOL "Add Test Coverage")
