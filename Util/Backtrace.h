/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Backtrace.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/05/05
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef __BACKTRACE_H__
#define __BACKTRACE_H__

#include <string>

namespace Backtrace {

std::string DumpBacktrace(const int32_t totalFrames = 20);

} // namespace Backtrace

#endif // __BACKTRACE_H__