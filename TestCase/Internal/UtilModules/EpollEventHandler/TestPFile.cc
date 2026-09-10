/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestPFile.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : PFile 文件事件封装内部测试
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
#include "PFile.h"
#include "gtest/gtest.h"

namespace {
std::string MakeFilePath(const std::string& prefix)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    char buf[96] = {};
    std::snprintf(buf, sizeof(buf), "/tmp/%s_%d_%ld.tmp", prefix.c_str(), getpid(), ts.tv_nsec % 1000000L);
    return std::string(buf);
}
}

class UtilModules_PFile : public ::testing::Test {
protected:
    void SetUp() override {
        mPath = MakeFilePath("spr_pfile");
    }

    void TearDown() override {
        unlink(mPath.c_str());
    }

    std::string mPath;
};

// 测试通过文件名构造文件事件
TEST_F(UtilModules_PFile, ConstructWithFileName)
{
    PFile file(mPath);
    EXPECT_TRUE(file.IsReady());
    EXPECT_GE(file.GetEvtFd(), 0);
}

// 测试构造不存在的文件路径
TEST_F(UtilModules_PFile, ConstructWithMissingFile)
{
    PFile file("/tmp/no_such_pfile_dir/no_file.tmp");
    EXPECT_FALSE(file.IsReady());
}

// 测试通过文件描述符构造文件事件
TEST_F(UtilModules_PFile, ConstructWithFd)
{
    int32_t fd = open(mPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    ASSERT_GE(fd, 0);

    PFile file(fd, [](int32_t, void*) {});
    EXPECT_GE(file.GetEvtFd(), 0);
    // PFile 析构会 Close fd，这里不再 close(fd)
}

// 测试文件事件回调被触发
TEST_F(UtilModules_PFile, EpollEventCallbackInvoked)
{
    int32_t hit = 0;
    PFile file(mPath, [&hit](int32_t, ssize_t, std::string, void*) {
        hit++;
    });

    file.EpollEvent(file.GetEvtFd(), EPOLL_TYPE_FILE, nullptr);
    EXPECT_GE(hit, 1);
}

// 测试带 fd 回调的文件事件被触发
TEST_F(UtilModules_PFile, EpollEventWithFdCallbackInvoked)
{
    int32_t fd = open(mPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    ASSERT_GE(fd, 0);

    int32_t hit = 0;
    PFile file(fd, [&hit](int32_t, void*) {
        hit++;
    });

    file.EpollEvent(file.GetEvtFd(), EPOLL_TYPE_FILE, nullptr);
    EXPECT_EQ(hit, 1);
}

// 测试非法 fd 的文件事件不崩溃
TEST_F(UtilModules_PFile, EpollEventInvalidFd)
{
    PFile file(mPath);
    // 传入错误的 fd 走错误日志分支，不应崩溃
    file.EpollEvent(-999, EPOLL_TYPE_FILE, nullptr);
}
