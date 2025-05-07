/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestBacktrace.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/05/15
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/05/15 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <string>
#include "gtest/gtest.h"
#include "Backtrace.h"

using namespace Backtrace;

// 测试 SprProcInfo::DumpBackTrace 方法
TEST(Util_Backtrace, DumpBackTraceTest) {
    std::string result = DumpBacktrace();

    EXPECT_FALSE(result.empty());
    std::istringstream iss(result);
    std::string line;
    if (std::getline(iss, line)) {
        EXPECT_EQ(line[0], '#');
    }
}
