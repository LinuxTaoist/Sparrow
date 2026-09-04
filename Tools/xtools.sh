#!/bin/bash

# ANSI escape codes for colors
BLACK='\033[0;30m'        # Black
RED='\033[0;31m'          # Red
GREEN='\033[0;32m'        # Green
BROWN='\033[0;33m'        # Brown (or Yellow on some terminals)
YELLOW='\033[1;33m'       # Bold Yellow
BLUE='\033[0;34m'         # Blue
PURPLE='\033[0;35m'       # Purple (or Magenta)
CYAN='\033[0;36m'         # Cyan
WHITE='\033[0;37m'        # White
LGREY='\033[0;37m'        # Light Grey (same as white on many terminals)
DGREY='\033[1;30m'        # Dark Grey
LBLUE='\033[1;34m'        # Light Blue
LPURPLE='\033[1;35m'      # Light Purple (or Light Magenta)
LCYAN='\033[1;36m'        # Light Cyan
LRED='\033[1;31m'         # Light Red
LGREEN='\033[1;32m'       # Light Green
LYELLOW='\033[1;33m'      # Light Yellow
LBROWN='\033[0;93m'       # Light Brown (or Gold)
DBLUE='\033[0;34m'        # Dark Blue

# No Color
NC='\033[0m'

# Capture start time
start_time=$(date +%s)

## project path
project_path=$(pwd)/..

# Delimiter
delimiter="================================================================================"

# Function to display completion details
display_completion_details() {
    end_time=$(date +%s)
    duration=$((end_time - start_time))

    echo ""
    echo -e "${GREEN}================================================================================${NC}"
    if [ "${1:-0}" -eq 0 ]; then
        echo -e "${GREEN}所有操作已完成。${NC}"
    else
        echo -e "${RED}操作失败，退出码：${1}${NC}"
    fi
    echo -e "${GREEN}完成时间：$(date '+%Y-%m-%d %H:%M:%S') 耗时 ${duration} 秒。${NC}"
    echo ""
}

# root path
root_path=$(pwd)/..

# cmd env
show_env() {
    # 显示当前环境变量
    echo -e "${PURPLE}==========================    Display  Environment    ==========================${NC}"
    # 显示Ubuntu版本
    echo -e "${PURPLE}- Ubuntu version: $(lsb_release -ds) ${NC}"
    # 显示g++版本
    echo -e "${PURPLE}- g++ version:    $(g++ --version | head -n1) ${NC}"
    # 显示gcc版本
    echo -e "${PURPLE}- gcc version:    $(gcc --version | head -n1) ${NC}"
    # 显示cmake版本
    echo -e "${PURPLE}- cmake version:  $(cmake --version | head -n1) ${NC}"
    echo -e "${PURPLE}${delimiter}${NC}"
}

# cmd adb_push
adb_push() {
    if [ -z "$1" ] || [ -z "$2" ]; then
        echo -e "${PURPLE}错误: 未提供必要的参数! 用法: ./xtool.sh <platform> <path>${NC}"
        return 1
    fi

    echo -e "${PURPLE} 开始推送文件 ${NC}"
    adb shell killall -10 servicemanagersrv
    sleep 2
    adb shell rm -rf $2/Release/Etc/*
    adb shell rm -rf $2/Release/Bin/*
    adb shell rm -rf $2/Release/Lib/*
    adb shell mkdir -p $2/Release
    adb push ../Release/$1/Etc  $2/Release/
    adb push ../Release/$1/Bin  $2/Release/
    adb push ../Release/$1/Lib  $2/Release/
}

# cmd commit-template
config_commit_template() {
    template_path=$(pwd)/../.git-commit-template
    echo ""
    echo -e "${PURPLE}git config --global commit.template ${template_path} ${NC}"
    git config --global commit.template ${template_path}
}

# cmd build-all
build_project() {
    echo -e "${GREEN}开始编译项目...${NC}"
    tools_path=$(pwd)
    build_3rdparty
    cd ${tools_path}/../Build/
    ./general_build.sh
}

## cmd build_3rd
build_3rdparty() {
    tools_path=$(pwd)
    echo -e "${PURPLE}= 开始编译libgo.... ${NC}"
    cd ${tools_path}/../3rdParty/libgo/
    ./build.sh

    echo -e "${PURPLE}= 开始编译3rdParty.... ${NC}"
    cd ${tools_path}/../3rdParty/sqlite/
    ./build.sh
}

## cmd new-platform
new_platform() {
    echo -e "${GREEN}开始搭建新项目...${NC}"
    platform_name="$1"
    cd ${project_path}

    echo -e "${GREEN}touch ${project_path}/Platform/${platform_name}/build_${platform_name}.sh ${NC}"
    mkdir -p ${project_path}/Platform/${platform_name}
    mkdir -p ${project_path}/Platform/${platform_name}/Build
    mkdir -p ${project_path}/Platform/${platform_name}/Build/Options
    touch    ${project_path}/Platform/${platform_name}/Build/Options/${platform_name}_compile_options.cmake
    touch    ${project_path}/Platform/${platform_name}/Build/Options/${platform_name}_modules_config.cmake
    touch    ${project_path}/Platform/${platform_name}/Build/build_${platform_name}.sh
    touch    ${project_path}/Platform/${platform_name}/Build/rebuild_${platform_name}.sh

    echo -e "${GREEN}touch ${project_path}/Platform/${platform_name}/Configs/vendor.prop ${NC}"
    mkdir -p ${project_path}/Platform/${platform_name}/Configs
    touch    ${project_path}/Platform/${platform_name}/Configs/vendor.prop
    touch    ${project_path}/Platform/${platform_name}/Configs/sprlog.conf
    echo "ro.vendor.platform.name=${platform_name}" > ${project_path}/Platform/${platform_name}/Configs/vendor.prop

    echo -e "${GREEN}mkdir -p ${project_path}/Platform/${platform_name}/3rdParty ${NC}"
    mkdir -p ${project_path}/Platform/${platform_name}/3rdParty
}

## cmd static_scan
static_scan() {
    echo -e "${GREEN}开始静态代码扫描...${NC}"
    cd $(pwd)/cppcheck
    ./RunCppcheck.sh
}

## start_valgrind
start_valgrind() {
    echo -e "${GREEN}开始启动valgrind...${NC}"
    cd $(pwd)/valgrind
    ./start_valgrind.sh
}

## stop_valgrind
stop_valgrind() {
    echo -e "${GREEN}开始停止valgrind...${NC}"
    cd $(pwd)/valgrind
    ./stop_valgrind.sh
}

## sync_code
sync_code() {
    echo -e "${PURPLE}开始同步代码仓库...${NC}"
    cd "${project_path}" || return 1

    if [ -d ".repo" ]; then
        echo -e "${GREEN}检测到 repo 管理环境，批量同步所有子仓库${NC}"
        repo sync --current-branch
    else
        if [ ! -d ".git" ]; then
            echo -e "${RED}错误：${project_path} 不是 Git 仓库${NC}"
            return 1
        fi
        echo -e "${GREEN}单仓库环境，同步当前仓库${NC}"
        git pull --ff-only
    fi
    local sync_status=$?

    if [ ${sync_status} -eq 0 ]; then
        echo -e "${GREEN}代码同步完成${NC}"
    else
        echo -e "${RED}同步失败，请检查上方具体仓库的错误信息${NC}"
    fi

    return ${sync_status}
}

# Function to print usage information with logo
usage() {
    local logo="\
                XTools - Sparrow 开发辅助工具
                                                     +++
     +++    +++  +++++++++++                         +++
     ++++  ++++  +++++++++++                         +++
      +++  +++       +++                             +++
       ++++++        +++        +++++       +++++    +++    +++++
       ++++++        +++      ++++++++    ++++++++   +++  +++++++
        ++++         +++      +++++++++   +++++++++  +++  +++   +
        ++++         +++     ++++   +++  ++++   +++  +++  +++
       ++++++        +++     +++    +++  +++    +++  +++  ++++++
       ++++++        +++     +++    +++  +++    +++  +++    +++++
      +++  +++       +++     ++++  ++++  ++++  ++++  +++       +++
     ++++  ++++      +++      ++++++++    ++++++++   +++  +    +++
     +++    +++      +++      ++++++++    ++++++++   +++  +++++++
    ++++    ++++     +++        ++++        ++++     +++  +++++
    "
    echo -e "${LPURPLE}================================================================================${NC}"
    echo -e "${LPURPLE}${logo}${NC}"
    echo -e "${LPURPLE}================================================================================${NC}"
    echo -e ""
    echo -e "${PURPLE}Usage:${NC}"
    echo -e "${PURPLE}  $0 env                      查看当前环境${NC}"
    echo -e "${PURPLE}  $0 adb-push <path>          adb push文件到设备${NC}"
    echo -e "${PURPLE}  $0 commit-template          配置commit模板${NC}"
    echo -e "${PURPLE}  $0 build-all                编译整个项目${NC}"
    echo -e "${PURPLE}  $0 build-3rd                编译依赖的第三方库${NC}"
    echo -e "${PURPLE}  $0 new-platform <platform>  创建新项目${NC}"
    echo -e "${PURPLE}  $0 staticscan               执行静态代码扫描${NC}"
    echo -e "${PURPLE}  $0 start-valgrind           启动valgrind${NC}"
    echo -e "${PURPLE}  $0 stop-valgrind            停止valgrind${NC}"
    echo -e "${PURPLE}  $0 help                     显示此帮助信息${NC}"
    echo -e "${PURPLE}  $0 sync-code                同步代码仓库${NC}"
    echo -e ""
    echo -e "${PURPLE}================================================================================${NC}"
}

# Main function to handle command-line arguments and dispatch commands
main() {
    if [ $# -eq 0 ]; then
        clear
        usage
        exit 1
    fi

    case "$1" in
        env)
            show_env
            ;;
        adb-push)
            adb_push "$2" "$3"
            ;;
        commit-template)
            config_commit_template
            ;;
        build-all)
            build_project;;
        build-3rd)
            build_3rdparty
            ;;
        new-platform)
            new_platform "$2";;
        staticscan)
            static_scan
            ;;
        start-valgrind)
            start_valgrind
            ;;
        stop-valgrind)
            stop_valgrind
            ;;
        sync-code)
            sync_code
            ;;
        help|?)
            usage
            ;;
        *)
            echo -e "${RED}未知命令: $1${NC} "
            echo -e "${RED}请使用 \"$0 help\" 命令查看帮助信息。${NC}"
            exit 1
            ;;
    esac

    command_status=$?
    display_completion_details ${command_status}
    return ${command_status}
}

# Call the main function with all passed arguments
main "$@"
