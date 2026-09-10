/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprMQueueDetails.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprMQueueDetails 消息队列详情共享内存内部测试
 *  @date       : 2026/09/09
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/09 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <ctime>
#include <string>
#include <cstdio>
#include <cerrno>
#include <mqueue.h>
#include <unistd.h>
#include "CommonMacros.h"
#include "CommonTypeDefs.h"
#include "SprMQueueDetails.h"
#include "gtest/gtest.h"

namespace {
std::string MakeShortName(const std::string& prefix)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    char buf[MQ_NAME_MAX_LENGTH] = {};
    std::snprintf(buf, sizeof(buf), "%s_%d_%ld", prefix.c_str(), getpid(), ts.tv_nsec % 100000L);
    return std::string(buf);
}

std::string BuildDetailsPath(const std::string& name)
{
    return std::string(DEFAULT_MQS_DIR) + "/" + name;
}
}

// 测试创建者与访问者共享同一份详情数据
TEST(Core_SprMQueueDetails, CreatorAndVisitorShareSameDetails)
{
    const std::string mqName = MakeShortName("utd");

    {
        SprMQueueDetails creator(mqName, true);
        ASSERT_EQ(0, creator.SetHandle(101));
        ASSERT_EQ(0, creator.SetMsgLenPeak(256));
        ASSERT_EQ(0, creator.SetLastMsgID(0xABCD1234U));
        ASSERT_EQ(0, creator.SetUsedPeak(4));
        ASSERT_EQ(1, creator.IncrementMsgTotal());
        ASSERT_EQ(2, creator.IncrementMsgTotal());

        int32_t handle = 0;
        int32_t lenPeak = 0;
        int32_t msgTotal = 0;
        int32_t usedPeak = 0;
        uint32_t lastMsgId = 0;
        std::string name;

        EXPECT_EQ(0, creator.GetHandle(handle));
        EXPECT_EQ(0, creator.GetMsgLenPeak(lenPeak));
        EXPECT_EQ(0, creator.GetMsgTotal(msgTotal));
        EXPECT_EQ(0, creator.GetLastMsgID(lastMsgId));
        EXPECT_EQ(0, creator.GetUsedPeak(usedPeak));
        EXPECT_EQ(0, creator.GetMQName(name));

        EXPECT_EQ(101, handle);
        EXPECT_EQ(256, lenPeak);
        EXPECT_EQ(2, msgTotal);
        EXPECT_EQ(4, usedPeak);
        EXPECT_EQ(0xABCD1234U, lastMsgId);
        EXPECT_EQ(mqName, name);

        SprMQueueDetails visitor(mqName, false);
        handle = 0;
        lenPeak = 0;
        msgTotal = 0;
        usedPeak = 0;
        lastMsgId = 0;
        name.clear();

        EXPECT_EQ(0, visitor.GetHandle(handle));
        EXPECT_EQ(0, visitor.GetMsgLenPeak(lenPeak));
        EXPECT_EQ(0, visitor.GetMsgTotal(msgTotal));
        EXPECT_EQ(0, visitor.GetLastMsgID(lastMsgId));
        EXPECT_EQ(0, visitor.GetUsedPeak(usedPeak));
        EXPECT_EQ(0, visitor.GetMQName(name));

        EXPECT_EQ(101, handle);
        EXPECT_EQ(256, lenPeak);
        EXPECT_EQ(2, msgTotal);
        EXPECT_EQ(4, usedPeak);
        EXPECT_EQ(0xABCD1234U, lastMsgId);
        EXPECT_EQ(mqName, name);
    }

    EXPECT_NE(0, access(BuildDetailsPath(mqName).c_str(), F_OK));
}

// 测试打开不存在的详情文件返回错误
TEST(Core_SprMQueueDetails, OpenMissingDetailsReturnsError)
{
    const std::string missingName = MakeShortName("missing");
    SprMQueueDetails visitor(missingName, false);

    int32_t handle = 0;
    std::string name = "init";

    EXPECT_EQ(-1, visitor.SetHandle(10));
    EXPECT_EQ(-1, visitor.SetMsgLenPeak(20));
    EXPECT_EQ(-1, visitor.SetLastMsgID(0x11U));
    EXPECT_EQ(-1, visitor.SetUsedPeak(3));
    EXPECT_EQ(-1, visitor.IncrementMsgTotal());

    EXPECT_EQ(-1, visitor.GetHandle(handle));
    EXPECT_EQ(-1, visitor.GetMQName(name));
    EXPECT_TRUE(name.empty());
}

// 测试 GetMQDetails 通过真实队列句柄更新 mq_attr
TEST(Core_SprMQueueDetails, GetMQDetailsUpdatesAttrWithRealQueueHandle)
{
    const std::string detailsName = MakeShortName("attr");
    const std::string posixMqName = "/" + MakeShortName("pmq");

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 8;
    attr.mq_msgsize = 64;
    attr.mq_curmsgs = 0;

    mqd_t mqd = mq_open(posixMqName.c_str(), O_CREAT | O_RDWR, 0666, &attr);
    ASSERT_NE(static_cast<mqd_t>(-1), mqd) << "mq_open failed, errno=" << errno;

    {
        SprMQueueDetails details(detailsName, true);
        ASSERT_EQ(0, details.SetHandle(static_cast<int32_t>(mqd)));
        ASSERT_EQ(0, details.SetMsgLenPeak(12));
        ASSERT_EQ(0, details.SetLastMsgID(0x77U));
        ASSERT_EQ(0, details.SetUsedPeak(2));
        ASSERT_EQ(1, details.IncrementMsgTotal());

        SMQueueDetails snapshot = {};
        ASSERT_EQ(0, details.GetMQDetails(snapshot));
        EXPECT_EQ(static_cast<long>(attr.mq_maxmsg), snapshot.mqAttr.mq_maxmsg);
        EXPECT_EQ(static_cast<long>(attr.mq_msgsize), snapshot.mqAttr.mq_msgsize);
        EXPECT_EQ(12, snapshot.msgLenPeak);
        EXPECT_EQ(1, snapshot.msgTotal);
        EXPECT_EQ(0x77U, snapshot.lastMsgID);
        EXPECT_EQ(2, snapshot.usedPeak);
        EXPECT_STREQ(detailsName.c_str(), snapshot.mqName);
    }

    mq_close(mqd);
    mq_unlink(posixMqName.c_str());
}
