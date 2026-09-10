/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestPPipe.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : PPipe FIFO 管道事件封装内部测试
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
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "PPipe.h"
#include "gtest/gtest.h"

namespace {
std::string MakeFifoPath()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    char buf[96] = {};
    std::snprintf(buf, sizeof(buf), "/tmp/spr_ppipe_%d_%ld.fifo", getpid(), ts.tv_nsec % 1000000L);
    return std::string(buf);
}
}

class UtilModules_PPipe : public ::testing::Test {
protected:
    void SetUp() override {
        mFifoPath = MakeFifoPath();
    }

    void TearDown() override {
        unlink(mFifoPath.c_str());
    }

    std::string mFifoPath;
};

// 测试通过文件描述符构造管道事件
TEST_F(UtilModules_PPipe, ConstructWithFd)
{
    int32_t pipeFds[2] = {-1, -1};
    ASSERT_EQ(pipe(pipeFds), 0);

    PPipe pipe(pipeFds[0], [](ssize_t, std::string, void*) {});
    EXPECT_TRUE(pipe.IsReady());
    EXPECT_EQ(pipe.GetEvtFd(), pipeFds[0]);

    close(pipeFds[1]);
}

// 测试通过文件名构造命名管道
TEST_F(UtilModules_PPipe, ConstructWithFileName)
{
    PPipe pipe(mFifoPath, [](ssize_t, std::string, void*) {});
    EXPECT_TRUE(pipe.IsReady());
    EXPECT_GE(pipe.GetEvtFd(), 0);

    // 析构应清理 fifo 文件
    struct stat st;
    EXPECT_EQ(stat(mFifoPath.c_str(), &st), 0);
    EXPECT_TRUE(S_ISFIFO(st.st_mode));
}

// 测试管道事件读写往返并触发回调
TEST_F(UtilModules_PPipe, EpollEventReadWriteRoundtrip)
{
    int32_t pipeFds[2] = {-1, -1};
    ASSERT_EQ(pipe(pipeFds), 0);

    ssize_t recvSize = 0;
    std::string recvBytes;
    PPipe pipe(pipeFds[0], [&recvSize, &recvBytes](ssize_t size, std::string bytes, void*) {
        recvSize = size;
        recvBytes = bytes;
    });

    const std::string payload = "ppipe-test-data";
    ASSERT_EQ(write(pipeFds[1], payload.c_str(), payload.size()), static_cast<ssize_t>(payload.size()));

    pipe.EpollEvent(pipeFds[0], EPOLL_TYPE_PIPE, nullptr);
    EXPECT_EQ(recvSize, static_cast<ssize_t>(payload.size()));
    EXPECT_EQ(recvBytes, payload);

    close(pipeFds[1]);
}

// 测试非法 fd 的事件处理不崩溃
TEST_F(UtilModules_PPipe, EpollEventInvalidFd)
{
    int32_t pipeFds[2] = {-1, -1};
    ASSERT_EQ(pipe(pipeFds), 0);

    PPipe pipe(pipeFds[0], [](ssize_t, std::string, void*) {});
    pipe.EpollEvent(-999, EPOLL_TYPE_PIPE, nullptr);

    close(pipeFds[1]);
}

// 测试空回调的管道事件处理
TEST_F(UtilModules_PPipe, EpollEventWithoutCallback)
{
    PPipe pipe(mFifoPath);
    ASSERT_TRUE(pipe.IsReady());

    // 无回调，事件处理不应崩溃
    pipe.EpollEvent(pipe.GetEvtFd(), EPOLL_TYPE_PIPE, nullptr);
}
