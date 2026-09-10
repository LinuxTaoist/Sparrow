/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestBinder.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Binder/IBinder 请求响应 Parcel 创建内部测试
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
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <memory>
#include <semaphore.h>
#include "Binder.h"
#include "IBinder.h"
#include "gtest/gtest.h"

namespace {
std::string MakeName(const std::string& prefix)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    char buf[96] = {};
    std::snprintf(buf, sizeof(buf), "%s_%d_%ld", prefix.c_str(), getpid(), ts.tv_nsec % 1000000L);
    return std::string(buf);
}
}

class Util_Binder : public ::testing::Test {
protected:
    void SetUp() override {
        mName = MakeName("binder");
        mKey = 10086;
    }

    void TearDown() override {
        // Parcel 底层用普通文件 + 信号量，这里清理残留
        unlink(("/tmp/" + mName + "_req").c_str());
        unlink(("/tmp/" + mName + "_rsp").c_str());

        // slave(IBinder) 不负责 sem_unlink（仅 master 清理），slave 单测需显式清理
        sem_unlink((mName + "_req" + std::to_string(mKey)).c_str());
        sem_unlink((mName + "_rsp" + std::to_string(mKey)).c_str());
    }

    std::string mName;
    int32_t mKey;
};

// 测试 Binder（服务端）创建 master Parcel
TEST_F(Util_Binder, GetParcelCreatesMasterParcels)
{
    Binder binder(mName, mKey);
    std::shared_ptr<Parcel> req;
    std::shared_ptr<Parcel> rsp;

    EXPECT_EQ(binder.GetParcel(req, rsp), 0);
    ASSERT_TRUE(req != nullptr);
    ASSERT_TRUE(rsp != nullptr);
}

// 测试 IBinder（客户端）创建 slave Parcel
TEST_F(Util_Binder, GetParcelCreatesSlaveParcels)
{
    IBinder binder(mName, mKey);
    std::shared_ptr<Parcel> req;
    std::shared_ptr<Parcel> rsp;

    EXPECT_EQ(binder.GetParcel(req, rsp), 0);
    ASSERT_TRUE(req != nullptr);
    ASSERT_TRUE(rsp != nullptr);
}
