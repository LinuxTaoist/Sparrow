/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2026  <dx_65535@163.com>.
 *
 *  @file       : TestInternalEnvironment.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Internal test common environment.
 *  @date       : 2026/09/21
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "gtest/gtest.h"

namespace {

class TestInternalEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
    }
};

::testing::Environment* const gTestInternalEnvironment =
    ::testing::AddGlobalTestEnvironment(new TestInternalEnvironment());

} // namespace
