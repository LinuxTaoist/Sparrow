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
    echo -e "${GREEN}所有操作已完成。${NC}"
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
    echo -e "${GREEN}touch ${project_path}/Build/${platform_name}/${platform_name}_build.sh ${NC}"
    mkdir -p ${project_path}/Build/options/${platform_name}
    touch ${project_path}/Build/options/${platform_name}/${platform_name}_compile_options.cmake
    touch ${project_path}/Build/${platform_name}_build.sh

    echo -e "${GREEN}touch ${project_path}/ProjectConfigs/Vendor/${platform_name}/vendor.prop ${NC}"
    mkdir -p ${project_path}/ProjectConfigs/Vendor/${platform_name}
    touch ${project_path}/ProjectConfigs/Vendor/${platform_name}/vendor.prop
    echo "ro.vendor=${platform_name}" > ${project_path}/ProjectConfigs/Vendor/${platform_name}/vendor.prop
}

# cmd static_scan
static_scan() {
    echo -e "${GREEN}开始静态代码扫描...${NC}"
    cd $(pwd)/../StaticScans
    ./RunCppcheck.sh
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
    echo -e "${PURPLE}  $0 commit-template          配置commit模板${NC}"
    echo -e "${PURPLE}  $0 build-all                编译整个项目${NC}"
    echo -e "${PURPLE}  $0 build-3rd                编译依赖的第三方库${NC}"
    echo -e "${PURPLE}  $0 new-platform <platform>  创建新项目${NC}"
    echo -e "${PURPLE}  $0 staticscan               执行静态代码扫描${NC}"
    echo -e "${PURPLE}  $0 help                     显示此帮助信息${NC}"
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
        help|?)
            usage
            ;;
        *)
            echo -e "${RED}未知命令: $1${NC} "
            echo -e "${RED}请使用 \"$0 help\" 命令查看帮助信息。${NC}"
            exit 1
            ;;
    esac

    display_completion_details
}

# Call the main function with all passed arguments
main "$@"
