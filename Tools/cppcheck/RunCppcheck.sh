#!/bin/bash

#######################################
# 路径配置变量（修改这里即可调整输出位置）
#######################################
# 输出目录（静态扫描结果存放的文件夹）
OUTPUT_DIR="."
# 输出文件前缀（如"StaticScan"，可自定义）
FILE_PREFIX="StaticScan"
# 输出文件格式（目前是xml，如需其他格式可修改）
FILE_EXT="xml"

#######################################
# 初始化与准备工作
#######################################
# 获取当前日期（YYYYMMDD）
DATE=$(date +%Y%m%d)

# 初始化序号为两位数（01）
SEQUENCE_NUMBER="01"

# 查找当天已存在的扫描文件（匹配格式：${FILE_PREFIX}_${DATE}_[0-9][0-9].${FILE_EXT}）
EXISTING_FILES=$(find "${OUTPUT_DIR}" -maxdepth 1 -type f -name "${FILE_PREFIX}_${DATE}_[0-9][0-9].${FILE_EXT}" -printf "%f\n" 2>/dev/null)

if [ -n "${EXISTING_FILES}" ]; then
    # 提取最后一个文件的序号（按版本号排序，取最大的）
    LAST_SEQUENCE=$(echo "${EXISTING_FILES}" | sort -Vr | head -1 | sed -E "s/${FILE_PREFIX}_${DATE}_([0-9]{2})\.${FILE_EXT}/\1/")
    # 计算下一个序号（转为数字+1后，补零为两位数）
    NEXT_SEQUENCE=$((10#${LAST_SEQUENCE} + 1))  # 10#强制十进制，避免08/09被当作八进制报错
    SEQUENCE_NUMBER=$(printf "%02d" "${NEXT_SEQUENCE}")  # 补零为两位数（如5→05）
fi

# 构建完整输出文件名（路径+前缀+日期+序号+后缀）
OUTPUT_FILENAME="${OUTPUT_DIR}/${FILE_PREFIX}_${DATE}_${SEQUENCE_NUMBER}.${FILE_EXT}"

#######################################
# 工程与Cppcheck配置
#######################################
# 工程根目录（以脚本位置为基准，使用规范化绝对路径）
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
# 验证工程根目录是否存在
if [ ! -d "${PROJECT_ROOT}" ]; then
    echo "Error: 工程根目录${PROJECT_ROOT}不存在！"
    exit 1
fi

# 指定待检查的源代码及头文件目录列表
SOURCE_DIRS="${PROJECT_ROOT}/3rdAdapter/    \
             ${PROJECT_ROOT}/Components/    \
             ${PROJECT_ROOT}/Core/          \
             ${PROJECT_ROOT}/Debug/         \
             ${PROJECT_ROOT}/Examples/      \
             ${PROJECT_ROOT}/Hardware/      \
             ${PROJECT_ROOT}/PrivateAPIs    \
             ${PROJECT_ROOT}/PublicAPIs/    \
             ${PROJECT_ROOT}/Util/          \
             ${PROJECT_ROOT}/UtilModules"

SOURCE_DIRS_INCLUDE=$(echo "${SOURCE_DIRS}" | sed 's/  */ -I/g' | sed 's/^-I//')

# 额外头文件搜索目录；用于补充源目录之外、Cppcheck 无法定位的项目头文件。
EXTRA_INCLUDES=(
)

INCLUDE_DIRS="${SOURCE_DIRS_INCLUDE} ${EXTRA_INCLUDES[*]}"

CPPCHECK_OPTS=(
    --std=c++11
    --enable=all
    --suppress=unusedFunction
    --suppress=useInitializationList
    --error-exitcode=1
    --xml
    --xml-version=2
    ${INCLUDE_DIRS}
    --output-file="${OUTPUT_FILENAME}"
)

# 不扫描的第三方代码和测试目录
CHECK_EXCLUDES=(
    -i "${PROJECT_ROOT}/3rdParty"
    -i "${PROJECT_ROOT}/TestCase"
    -i "${PROJECT_ROOT}/Util/cJSON.c"
    -i "${PROJECT_ROOT}/Util/cJSON.h"
    -i "${PROJECT_ROOT}/UtilModules/CodecX/cJSON.c"
    -i "${PROJECT_ROOT}/UtilModules/CodecX/cJSON.h"
    "--suppress=*:${PROJECT_ROOT}/Util/cJSON.*"
    "--suppress=*:${PROJECT_ROOT}/UtilModules/CodecX/cJSON.*"
    "--suppress=syntaxError:${PROJECT_ROOT}/Components/Business/OneNetMqtt/Src/OneNetCommon.h"
)

#######################################
# 检测Cppcheck头文件缺失
#######################################
echo "======================================"
echo "开始检测头文件缺失情况（--check-config）"
echo "======================================"
# 保存配置检查输出并显示处理进度，供后续判断是否存在缺失头文件。
CHECK_CONFIG_LOG=$(mktemp)
trap 'rm -f "${CHECK_CONFIG_LOG}"' EXIT
cppcheck --check-config "${CPPCHECK_OPTS[@]}" "${CHECK_EXCLUDES[@]}" ${SOURCE_DIRS} 2>&1 |
    tee "${CHECK_CONFIG_LOG}" |
    awk '
        BEGIN { checked = 0; progress = 0 }
        /^Checking / {
            checked++
            printf("\r头文件配置检查进度：已处理 %d 项", checked)
            fflush()
            progress = 1
            next
        }
        {
            if (progress) {
                printf("\n")
                progress = 0
            }
            print
        }
        END { if (progress) printf("\n") }
    '
MISSING_INCLUDES=$(grep "missingInclude" "${CHECK_CONFIG_LOG}" || true)

if [ -n "${MISSING_INCLUDES}" ]; then
    echo -e "\n⚠️  发现缺失的头文件："
    echo "${MISSING_INCLUDES}"
    echo -e "\n❗ 解决方法："
    echo "1. 找到上述缺失头文件的实际路径（如\"third_party/log.h\"可能在\"${PROJECT_ROOT}/third_party/include/\"）"
    echo "2. 在脚本的\"EXTRA_INCLUDES\"数组中添加对应的目录（格式：-I\"目录路径\"）"
    echo -e "\n⚠️  提示：当前将继续执行静态检查，但结果可能不准确！"
    read -p "是否继续执行？（y/n，默认y）：" CONTINUE
    if [ "${CONTINUE}" != "y" ] && [ "${CONTINUE}" != "" ]; then
        echo "已取消执行静态检查"
        exit 0
    fi
else
    echo -e "\n✅ 未发现缺失的头文件，检查结果将更准确"
fi

#######################################
# 执行静态检查
#######################################
echo -e "\n======================================"
echo "开始执行Cppcheck静态检查"
echo "======================================"
echo "输出文件：${OUTPUT_FILENAME}"
cppcheck "${CPPCHECK_OPTS[@]}" "${CHECK_EXCLUDES[@]}" ${SOURCE_DIRS}
CPPCHECK_EXIT_CODE=$?
ACTIONABLE_COUNT=0
if [ -s "${OUTPUT_FILENAME}" ]; then
    ACTIONABLE_COUNT=$(grep -Ec 'severity="(error|warning)"' "${OUTPUT_FILENAME}" || true)
fi

# 信息级配置提示不计为代码问题；仅当报告存在且没有 error/warning 时判定成功。
if [ -s "${OUTPUT_FILENAME}" ] && [ "${CPPCHECK_EXIT_CODE}" -eq 0 ]; then
    echo -e "\n✅ 静态检查完成，结果已保存至${OUTPUT_FILENAME}"
elif [ -s "${OUTPUT_FILENAME}" ] && [ "${ACTIONABLE_COUNT}" -eq 0 ]; then
    echo -e "\n✅ 静态检查完成，结果已保存至${OUTPUT_FILENAME}"
else
    echo -e "\n❌ Error: 静态检查发现问题，详情见${OUTPUT_FILENAME}"
    exit 1
fi