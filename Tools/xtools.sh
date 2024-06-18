#!/bin/bash

# ANSI escape codes for colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Function to build the project
build_project() {
    echo -e "${GREEN}开始编译项目...${NC}"
    make clean && make
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}编译成功！${NC}"
    else
        echo -e "${RED}编译失败，请检查错误日志！${NC}"
    fi
}

# Function to perform static code analysis
static_scan() {
    echo -e "${GREEN}开始静态代码扫描...${NC}"
    cppcheck --enable=all --inconclusive src/
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}静态代码扫描完成，未发现重大问题！${NC}"
    else
        echo -e "${RED}静态代码扫描发现潜在问题，请查看报告！${NC}"
    fi
}

# Function to print usage information with logo
usage() {
    local logo="\
                                                                        +++
     +++++      +++++ ++++++++++++++++                                  +++
      ++++      ++++  ++++++++++++++++                                  +++
      +++++    +++++  ++++++++++++++++                                  +++
       ++++    ++++         ++++                                        +++
       +++++  ++++          ++++                                        +++
        ++++ +++++          ++++           ++++++          ++++++       +++      ++++++
        ++++ ++++           ++++         ++++++++++      ++++++++++     +++    ++++++++
         +++++++            ++++        ++++++++++++    ++++++++++++    +++   +++++++++
         +++++++            ++++        ++++    ++++    ++++    ++++    +++   ++++    +
          +++++             ++++       +++++     ++++  +++++     ++++   +++   ++++
         +++++++            ++++       ++++      ++++  ++++      ++++   +++   +++++++
         ++++++++           ++++       ++++      ++++  ++++      ++++   +++    ++++++++
        ++++ ++++           ++++       ++++      ++++  ++++      ++++   +++     ++++++++
       +++++ +++++          ++++       ++++      ++++  ++++      ++++   +++       ++++++
       ++++   ++++          ++++       ++++      ++++  ++++      ++++   +++         ++++
      +++++   +++++         ++++        ++++    ++++    ++++    ++++    +++   +     ++++
      ++++     +++++        ++++        ++++++++++++    ++++++++++++    +++   ++++++++++
     +++++     +++++        ++++         ++++++++++      ++++++++++     +++   +++++++++
    +++++       +++++       ++++           ++++++          ++++++       +++    ++++++
 "

    echo "========================================================================================="
    echo -e "${GREEN}${logo}${NC}"
    echo "========================================================================================="
    echo ""
    echo "DevTools - Linux C++ 中间件开发辅助工具"
    echo ""
    echo "Usage:"
    echo "  $0 <command>"
    echo ""
    echo "Commands:"
    echo "  build       编译整个项目"
    echo "  staticscan  执行静态代码扫描"
    echo "  help        显示此帮助信息"
}

# Main function to handle command-line arguments and dispatch commands
main() {
    if [ $# -eq 0 ]; then
        usage
        exit 1
    fi

    case "$1" in
        build)
            build_project
            ;;
        staticscan)
            static_scan
            ;;
        help)
            usage
            ;;
        *)
            echo -e "${RED}未知命令: $1${NC}"
            usage
            ;;
    esac
}

# Call the main function with all passed arguments
main "$@"