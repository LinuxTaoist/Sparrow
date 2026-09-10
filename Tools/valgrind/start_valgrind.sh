#!/bin/bash
set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
    start_valgrind.sh [--bin-path <dir>] [--report-path <dir>]

Options:
    --bin-path <dir>     服务可执行文件目录，默认当前路径
    --report-path <dir>  valgrind 报告输出目录，默认当前路径下 valgrind_logs
    -h, --help           显示帮助

说明:
    用 valgrind memcheck 启动 Sparrow 各服务，提前发现内存泄漏等问题。
EOF
}

BIN_PATH="$(pwd)"
REPORT_PATH="$(pwd)/valgrind_logs"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --bin-path)
            BIN_PATH="$2"
            shift 2
            ;;
        --bin-path=*)
            BIN_PATH="${1#*=}"
            shift
            ;;
        --report-path)
            REPORT_PATH="$2"
            shift 2
            ;;
        --report-path=*)
            REPORT_PATH="${1#*=}"
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "未知参数: $1" >&2
            usage
            exit 1
            ;;
    esac
done

# 服务列表（与 Configs/General/init.conf 对应）
SERVICES=(
    "logmanagersrv"
    "bindermanagersrv"
    "mediatorsrv"
    "propertiessrv"
    "configmanagersrv"
    "powermanagersrv"
    "sparrowsrv"
    "statusmonitorsrv"
)

# 校验服务目录
if [ ! -d "$BIN_PATH" ]; then
    echo "错误：服务目录不存在！BIN_PATH=$BIN_PATH" >&2
    exit 1
fi

mkdir -p "$REPORT_PATH"

PID_FILE="$REPORT_PATH/service_pids.txt"
: > "$PID_FILE"

echo "正在启动服务（valgrind memcheck）..."

idx=0
for service in "${SERVICES[@]}"; do
    idx=$((idx + 1))
    service_path="${BIN_PATH}/${service}"
    log_file="${REPORT_PATH}/valgrind_${service}.log"

    if [ ! -x "$service_path" ]; then
        echo "错误：服务不存在或不可执行，跳过！$service_path" >&2
        continue
    fi

    printf "  [%d/%d] %s\n" "$idx" "${#SERVICES[@]}" "$service"

    valgrind \
        --tool=memcheck \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --log-file="${log_file}" \
        "${service_path}" &

    echo "${service} $!" >> "$PID_FILE"
    sleep 2
done

echo ""
echo "所有服务启动完成，共 ${#SERVICES[@]} 个"
echo "报告目录: ${REPORT_PATH}"
