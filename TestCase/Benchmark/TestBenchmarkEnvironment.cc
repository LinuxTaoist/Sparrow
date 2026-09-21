/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : TestBenchmarkEnvironment.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Benchmark test common environment.
 *  @date       : 2026/09/21
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/21 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "gtest/gtest.h"

namespace {

class TestBenchmarkEnvironment : public ::testing::Environment {
public:
    void SetUp() override
    {
    }
};

::testing::Environment* const gTestBenchmarkEnvironment =
    ::testing::AddGlobalTestEnvironment(new TestBenchmarkEnvironment());

} // namespace
