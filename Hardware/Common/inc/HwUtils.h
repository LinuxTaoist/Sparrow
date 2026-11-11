/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HwUtils.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Internal POSIX command execution for HAL implementations.
 *  @date       : 2026/11/11
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __HW_UTILS_H__
#define __HW_UTILS_H__

#include <cstdint>
#include <string>

namespace HwUtils {

// Run a printf-formatted shell command; return 0 on success, otherwise -1.
int32_t RunCommand(const char* format, ...);
// Replace output with the command's stdout; stderr follows the command's redirection.
int32_t RunCommand(std::string& output, const char* format, ...);

}

#endif // __HW_UTILS_H__
