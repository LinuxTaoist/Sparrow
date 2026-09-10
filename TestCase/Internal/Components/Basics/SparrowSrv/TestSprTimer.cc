/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprTimer.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprTimer 定时器实体内部测试
 *  @date       : 2026/09/10
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/10 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprTimer.h"
#include "gtest/gtest.h"

// 测试构造与访问器
TEST(UtilModules_SprTimer, ConstructorAndAccessors)
{
    SprTimer timer(100, 0x1234, 3, 10, 20);

    EXPECT_EQ(timer.GetModuleId(), 100u);
    EXPECT_EQ(timer.GetMsgId(), 0x1234u);
    EXPECT_EQ(timer.GetIntervalInMilliSec(), 20u);
    EXPECT_EQ(timer.GetRepeatTimes(), 3u);
    EXPECT_EQ(timer.GetRepeatCount(), 0u);
    EXPECT_GT(timer.GetExpired(), 0u);
}

// 测试拷贝构造
TEST(UtilModules_SprTimer, CopyConstructor)
{
    SprTimer src(1, 2, 3, 4, 5);
    SprTimer dst(src);

    EXPECT_EQ(dst.GetModuleId(), src.GetModuleId());
    EXPECT_EQ(dst.GetMsgId(), src.GetMsgId());
    EXPECT_EQ(dst.GetIntervalInMilliSec(), src.GetIntervalInMilliSec());
    EXPECT_EQ(dst.GetExpired(), src.GetExpired());
    EXPECT_EQ(dst.GetRepeatTimes(), src.GetRepeatTimes());
}

// 测试移动构造
TEST(UtilModules_SprTimer, MoveConstructor)
{
    SprTimer src(1, 2, 3, 4, 5);
    SprTimer dst(std::move(src));

    EXPECT_EQ(dst.GetModuleId(), 1u);
    EXPECT_EQ(dst.GetMsgId(), 2u);
}

// 测试拷贝赋值
TEST(UtilModules_SprTimer, CopyAssignment)
{
    SprTimer src(1, 2, 3, 4, 5);
    SprTimer dst(9, 9, 9, 9, 9);
    dst = src;

    EXPECT_EQ(dst.GetModuleId(), src.GetModuleId());
    EXPECT_EQ(dst.GetMsgId(), src.GetMsgId());
}

// 测试移动赋值
TEST(UtilModules_SprTimer, MoveAssignment)
{
    SprTimer src(1, 2, 3, 4, 5);
    SprTimer dst(9, 9, 9, 9, 9);
    dst = std::move(src);

    EXPECT_EQ(dst.GetModuleId(), 1u);
    EXPECT_EQ(dst.GetMsgId(), 2u);
}

// 测试小于运算符（按过期时间比较）
TEST(UtilModules_SprTimer, LessThanOperator)
{
    SprTimer earlier(1, 0x100, 0, 0, 0);
    SprTimer later(1, 0x200, 0, 1000, 0);

    EXPECT_TRUE(earlier < later);
    EXPECT_FALSE(later < earlier);

    // 过期时间相同，按 msgId 比较
    SprTimer sameExpiredA(1, 0x100, 0, 0, 0);
    SprTimer sameExpiredB(1, 0x200, 0, 0, 0);
    EXPECT_TRUE(sameExpiredA < sameExpiredB);
    EXPECT_FALSE(sameExpiredB < sameExpiredA);
}

// 测试是否过期
TEST(UtilModules_SprTimer, IsExpired)
{
    SprTimer notExpired(1, 1, 0, 60000, 0);   // 60 秒后才过期
    EXPECT_FALSE(notExpired.IsExpired());

    SprTimer expired(1, 1, 0, 0, 0);          // 立即过期
    EXPECT_TRUE(expired.IsExpired());
}

// 测试获取当前时间
TEST(UtilModules_SprTimer, GetTickMs)
{
    SprTimer timer(1, 1, 0, 0, 0);
    EXPECT_GT(timer.GetTickMs(), 0u);
}

// 测试设置过期时间与重复计数
TEST(UtilModules_SprTimer, SetExpiredAndRepeatCount)
{
    SprTimer timer(1, 1, 0, 0, 0);

    timer.SetExpired(12345);
    EXPECT_EQ(timer.GetExpired(), 12345u);

    timer.RepeatCount();
    timer.RepeatCount();
    EXPECT_EQ(timer.GetRepeatCount(), 2u);
}
