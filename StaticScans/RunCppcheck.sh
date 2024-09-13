#!/bin/bash

# 获取当前日期（YYYYMMDD）
DATE=$(date +%Y%m%d)

# 初始化序号变量，并确保其始终为两位数（左侧补零）
SEQUENCE_NUMBER=01
PADDING='0'

# 查找当天已存在的文件数量，用于确定新的序号
EXISTING_FILES=$(find ../StaticScans/ -maxdepth 1 -type f -name "StaticScan_${DATE}_[0-9][0-9].xml" 2>/dev/null | wc -l)
if [ $? -ne 0 ]; then
    # 当前日期没有检查结果，则序号保持01不变
    :
else
    # 取最后一个文件名中的序号并加1作为新的序号，并补足两位数
    LAST_SEQUENCE=$(find ../StaticScans/ -maxdepth 1 -type f -name "StaticScan_${DATE}_???.xml" -printf "%f\n" | sort -Vr | head -1 | sed 's/StaticScan_'${DATE}'_\([0-9][0-9]\)\.xml/\1/')
    NEXT_SEQUENCE=$((LAST_SEQUENCE + 1))

    # 补足两位数
    if [ ${#NEXT_SEQUENCE} -eq 1 ]; then
        NEXT_SEQUENCE="${PADDING}${NEXT_SEQUENCE}"
    fi

    SEQUENCE_NUMBER=$NEXT_SEQUENCE
fi

# 构建输出文件名
OUTPUT_FILENAME="StaticScan_${DATE}_${SEQUENCE_NUMBER}.xml"

# 设置Cppcheck命令行参数
CPPCHECK_OPTS="--std=c++11 --enable=all --suppress=unusedFunction --suppress=useInitializationList --error-exitcode=1 --xml --xml-version=2 --output-file=../StaticScans/${OUTPUT_FILENAME}"

# 指定待检查的源代码及头文件目录列表
SOURCE_DIRS="../3rdAdapter/ ../Components/Basics/ ../Core/ ../Debug/ ../Examples/ ../Hardware/ ../PublicAPIs/ ../Util/"

# 执行Cppcheck静态代码检查，包括源文件和对应的头文件
cppcheck $CPPCHECK_OPTS $SOURCE_DIRS

# 输出完成消息
echo ""
echo "已完成对指定目录及其子目录下的C++源代码和头文件进行静态检查，并将结果输出到../StaticScans/${OUTPUT_FILENAME}文件。"
