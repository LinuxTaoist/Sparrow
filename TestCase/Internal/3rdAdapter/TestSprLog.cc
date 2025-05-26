/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprLog.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <vector>
#include "gtest/gtest.h"
#include "SprLog.h"

// 定义不同长度的 LOG_TAG
#define LOG_TAG "TestTag"
#define LONG_LOG_TAG "ThisIsAVeryLongTagThatExceedsLimit"

// 测试基本日志级别
TEST(ThirdAdapter_SprLog, BasicLoggingLevels) {
    // 测试 SPR_LOGD
    SPR_LOGD("This is a debug log");

    // 测试 SPR_LOGI
    SPR_LOGI("This is an info log");

    // 测试 SPR_LOGW
    SPR_LOGW("This is a warning log");

    // 测试 SPR_LOGE
    SPR_LOGE("This is an error log");
}

// 测试日志标签长度检查
TEST(ThirdAdapter_SprLog, TagLengthCheck) {
    // 测试正常长度标签
    SPR_LOGD("This is a log with normal tag length");

    // 尝试使用超长标签，这里期望编译失败，但我们可以通过注释和说明来强调
    // 如果在代码中直接使用下面的宏，会触发静态断言
    // #define LOG_TAG LONG_LOG_TAG
    // SPR_LOGD("This should fail due to long tag");
}

// 测试可变参数日志
TEST(ThirdAdapter_SprLog, VariableArgumentLogging) {
    int value = 42;
    SPR_LOGD("This is a log with variable argument: %d", value);
}

// 测试单例模式
TEST(ThirdAdapter_SprLog, SingletonPattern) {
    SprLog* instance1 = SprLog::GetInstance();
    SprLog* instance2 = SprLog::GetInstance();
    EXPECT_EQ(instance1, instance2);
}

// 测试1000条日志输出
TEST(ThirdAdapter_SprLog, MultipleLogs) {
    for (int i = 0; i <= 1000; ++i) {
        SPR_LOGD("Test log out times: %d", i);
    }
}

// 测试超长日志输出
TEST(ThirdAdapter_SprLog, LongLogOutput) {
    std::string longMessage(1024, 'a');
    SPR_LOGD("%s", longMessage.c_str());
}
