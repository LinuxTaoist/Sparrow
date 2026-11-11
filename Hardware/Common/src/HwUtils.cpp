/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwUtils.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Internal POSIX command execution for HAL implementations.
 *  @date       : 2026/11/11
 *---------------------------------------------------------------------------------------------------------------------
 */
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include "HwUtils.h"

namespace {

int32_t ExecuteCommand(std::string& output, const char* format, va_list args) {
    output.clear();
    if (!format) {
        return -1;
    }

    char* command = nullptr;
    int32_t size = vasprintf(&command, format, args);
    if (size < 0 || !command) {
        return -1;
    }

    FILE* pipe = popen(command, "re");
    std::free(command);
    if (!pipe) {
        return -1;
    }

    char buffer[256];
    while (std::fgets(buffer, sizeof(buffer), pipe)) {
        output.append(buffer);
    }
    bool hasError = std::ferror(pipe) != 0;
    int32_t status = pclose(pipe);
    return !hasError && status == 0 ? 0 : -1;
}

}

int32_t HwUtils::RunCommand(const char* format, ...) {
    std::string output;
    va_list args;
    va_start(args, format);
    int32_t ret = ExecuteCommand(output, format, args);
    va_end(args);
    return ret;
}

int32_t HwUtils::RunCommand(std::string& output, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int32_t ret = ExecuteCommand(output, format, args);
    va_end(args);
    return ret;
}
