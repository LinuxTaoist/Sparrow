/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Backtrace.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/05/05
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/05/05 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <iostream>
#include <sstream>
#include <iomanip>
#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>
#include <signal.h>
#include "Backtrace.h"

namespace Backtrace {

std::string DumpBacktrace(const int32_t totalFrames)
{
    std::ostringstream oss;

    void* pCallStacks[totalFrames] = {};
    int32_t numFrames = backtrace(pCallStacks, totalFrames);
    char** pSymbols = backtrace_symbols(pCallStacks, numFrames);

    for (int32_t i = 0; i < numFrames; ++i) {
        Dl_info info;
        std::string funcName;

        if (dladdr(pCallStacks[i], &info)) {
            int32_t status = -1;
            char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);
            if (demangled != nullptr) {
                funcName = demangled;
                free(demangled);
            } else if (info.dli_sname != nullptr) {
                funcName = info.dli_sname;
            } else {
                funcName = "??";
            }
        } else {
            funcName = (pSymbols != nullptr ? pSymbols[i] : "??");
        }

        oss << "#" << std::setw(2) << i << "  " << funcName << " at " << pCallStacks[i] << std::endl;
    }

    if (pSymbols != nullptr) {
        free(pSymbols);
    }

    if (numFrames >= totalFrames) {
        oss << "# [truncated]" << std::endl;
    }

    return oss.str();
}

} // namespace Backtrace
