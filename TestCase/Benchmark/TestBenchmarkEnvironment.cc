/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : TestBenchmarkEnvironment.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Benchmark test common environment.
 *  @date       : 2026/09/21
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "gtest/gtest.h"

namespace {

class TestBenchmarkEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
    }
};

::testing::Environment* const gTestBenchmarkEnvironment =
    ::testing::AddGlobalTestEnvironment(new TestBenchmarkEnvironment());

} // namespace
