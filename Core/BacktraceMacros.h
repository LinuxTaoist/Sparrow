/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BacktraceMacros.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/25
 *
 *  This header file serves as a repository for shared data structure types utilized by both client and server code.
 *  All common data structure definitions intended for use across both client and server should be encapsulated herein.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/07/27 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __BACKTRACE_MACROS_H__
#define __BACKTRACE_MACROS_H__

#include <string>
#include <sstream>
#include <signal.h>
#include <string.h>
#include "SprLog.h"
#include "Backtrace.h"

//---------------------------------------------------------------------------------------------------------------------
// - Common macros print backtrace
//--------------------------------------------------------------------------------------------------------------------
#ifndef PRINT_BACKTRACE
#undef PRINT_BACKTRACE
#endif
#define PRINT_BACKTRACE(signum, totalFrames) do {       \
    std::string line;                                               \
    std::istringstream iss(Backtrace::DumpBacktrace(totalFrames));  \
    SPR_LOGE("Receive signal %d, %s.", signum, strsignal(signum));  \
    while (std::getline(iss, line)) {                               \
        SPR_LOGE("%s", line.c_str());                               \
    }                                                               \
} while(0)

#endif // __BACKTRACE_MACROS_H__
