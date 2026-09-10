/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestParcel.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Parcel 共享内存+信号量进程间通信内部测试
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
#include <cstdio>
#include <unistd.h>
#include <string>
#include <vector>
#include "Parcel.h"
#include "gtest/gtest.h"

namespace {
std::string MakeParcelPath()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    char buf[64] = {};
    std::snprintf(buf, sizeof(buf), "test_parcel_%d_%ld", getpid(), ts.tv_nsec % 1000000L);
    return std::string(buf);
}
}

class Util_Parcel : public ::testing::Test {
protected:
    void SetUp() override {
        mPath = MakeParcelPath();
    }

    void TearDown() override {
        // Parcel 内部 SharedRingBuffer 用 open(O_CREAT) 在 /tmp 下创建普通文件，
        // 其析构仅 munmap 不删除文件，这里手动清理，避免副产物残留。
        unlink(("/tmp/" + mPath).c_str());
    }

    std::string mPath;
};

// 测试布尔类型往返通信
TEST_F(Util_Parcel, BoolRoundtrip)
{
    Parcel master(mPath, 1, true);
    Parcel slave(mPath, 1, false);

    EXPECT_EQ(master.WriteBool(true), 0);
    EXPECT_EQ(master.Post(), 0);

    bool value = false;
    EXPECT_EQ(slave.TimedWait(2000), 0);
    EXPECT_EQ(slave.ReadBool(value), 0);
    EXPECT_TRUE(value);

    EXPECT_EQ(master.WriteBool(false), 0);
    EXPECT_EQ(master.Post(), 0);
    EXPECT_EQ(slave.TimedWait(2000), 0);
    EXPECT_EQ(slave.ReadBool(value), 0);
    EXPECT_FALSE(value);
}

// 测试整型往返通信（含负数与边界值）
TEST_F(Util_Parcel, IntRoundtrip)
{
    Parcel master(mPath, 2, true);
    Parcel slave(mPath, 2, false);

    EXPECT_EQ(master.WriteInt(42), 0);
    EXPECT_EQ(master.WriteInt(-123456), 0);
    EXPECT_EQ(master.WriteInt(0x7FFFFFFF), 0);
    EXPECT_EQ(master.Post(), 0);

    int a = 0, b = 0, c = 0;
    EXPECT_EQ(slave.TimedWait(2000), 0);
    EXPECT_EQ(slave.ReadInt(a), 0);
    EXPECT_EQ(slave.ReadInt(b), 0);
    EXPECT_EQ(slave.ReadInt(c), 0);
    EXPECT_EQ(a, 42);
    EXPECT_EQ(b, -123456);
    EXPECT_EQ(c, 0x7FFFFFFF);
}

// 测试字符串往返通信
TEST_F(Util_Parcel, StringRoundtrip)
{
    Parcel master(mPath, 3, true);
    Parcel slave(mPath, 3, false);

    const std::string text = "hello parcel string payload";
    EXPECT_EQ(master.WriteString(text), 0);
    EXPECT_EQ(master.Post(), 0);

    std::string out;
    EXPECT_EQ(slave.TimedWait(2000), 0);
    EXPECT_EQ(slave.ReadString(out), 0);
    EXPECT_EQ(out, text);
}

// 测试原始二进制数据往返通信
TEST_F(Util_Parcel, DataRoundtrip)
{
    Parcel master(mPath, 4, true);
    Parcel slave(mPath, 4, false);

    const std::vector<uint8_t> payload = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02};
    EXPECT_EQ(master.WriteData(const_cast<uint8_t*>(payload.data()), payload.size()), 0);
    EXPECT_EQ(master.Post(), 0);

    uint8_t buf[16] = {};
    int size = 0;
    EXPECT_EQ(slave.TimedWait(2000), 0);
    EXPECT_EQ(slave.ReadData(buf, size), 0);
    EXPECT_EQ(size, static_cast<int>(payload.size()));
    for (size_t i = 0; i < payload.size(); ++i) {
        EXPECT_EQ(buf[i], payload[i]);
    }
}

// 测试向量容器往返通信
TEST_F(Util_Parcel, VectorRoundtrip)
{
    Parcel master(mPath, 5, true);
    Parcel slave(mPath, 5, false);

    const std::vector<int32_t> vec = {10, -20, 30, -40, 50};
    EXPECT_EQ(master.WriteVector(vec), 0);
    EXPECT_EQ(master.Post(), 0);

    std::vector<int32_t> out;
    EXPECT_EQ(slave.TimedWait(2000), 0);
    EXPECT_EQ(slave.ReadVector(out), 0);
    ASSERT_EQ(out.size(), vec.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        EXPECT_EQ(out[i], vec[i]);
    }
}

// 测试无数据时定时等待超时
TEST_F(Util_Parcel, TimedWaitTimeoutWhenNoData)
{
    Parcel master(mPath, 6, true);
    Parcel slave(mPath, 6, false);

    // 无数据时 TimedWait 应超时
    EXPECT_EQ(slave.TimedWait(50), -301);
}

// 测试无读者时 Post 递增信号量成功
TEST_F(Util_Parcel, PostWithoutReader)
{
    Parcel master(mPath, 7, true);
    // Post 没有读者也能成功（信号量递增）
    EXPECT_EQ(master.Post(), 0);
    EXPECT_EQ(master.Post(), 0);
}
