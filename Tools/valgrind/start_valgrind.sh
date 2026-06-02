#!/bin/bash

# -------------------------- 配置区（根据实际情况修改）--------------------------
# 服务二进制文件（bin）的路径（绝对路径或相对于当前脚本执行目录的相对路径）
# 示例：
# BIN_PATH="/opt/my_project/bin"    # 绝对路径
# BIN_PATH="../../Release/Bin/"     # 相对路径（脚本所在目录的上一级bin目录）
BIN_PATH="../../Release/Bin/"       # 默认当前目录下的bin子目录，根据实际情况修改
# ------------------------------------------------------------------------------

# 定义服务列表（与你的配置文件对应）
SERVICES=(
  "logmanagersrv"
  "bindermanagersrv"
  "mediatorsrv"
  "propertiessrv"
  "powermanagersrv"
  "sparrowsrv"
  "statusmonitorsrv"
)

# 进入目标路径
cd $BIN_PATH

# 定义Valgrind日志存放目录（可自定义）
LOG_DIR="./valgrind_logs"
mkdir -p $LOG_DIR  # 确保日志目录存在


# 检查BIN_PATH是否存在
if [ ! -d "$BIN_PATH" ]; then
  echo "错误：服务二进制文件路径不存在！BIN_PATH=$BIN_PATH"
  exit 1
fi

# 循环启动每个服务
for service in "${SERVICES[@]}"; do
  # 拼接服务完整路径
  service_path="${BIN_PATH}/${service}"

  # 检查服务文件是否存在且可执行
  if [ ! -x "$service_path" ]; then
    echo "错误：服务文件不存在或不可执行！路径=$service_path"
    exit 1
  fi

  # 每个服务的Valgrind日志文件
  log_file="${LOG_DIR}/valgrind_${service}.log"

  echo "Starting ${service} (路径: ${service_path})... 日志: ${log_file}"

  # 用Valgrind启动服务（后台运行）
  valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --log-file="${log_file}" \
    "${service_path}" &  # 若服务需要参数，添加在后面（如 "${service_path}" --config=xxx）

  # 记录服务PID（用于停止脚本）
  echo "${service} PID: $!" >> "${LOG_DIR}/service_pids.txt"
  sleep 2
done

echo "所有服务启动完成，日志路径：${LOG_DIR}"
