/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSprDirWatch.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : SprDirWatch 目录监控内部测试
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
#include <poll.h>
#include <cstdio>
#include <vector>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/inotify.h>
#include "SprDirWatch.h"
#include "gtest/gtest.h"

namespace {
bool WaitForMaskEvent(int32_t fd, uint32_t expectedMask, const std::string& expectedName)
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    int32_t pollRet = poll(&pfd, 1, 1200);
    if (pollRet <= 0 || (pfd.revents & POLLIN) == 0) {
        return false;
    }

    char buffer[4096] = {};
    ssize_t bytes = read(fd, buffer, sizeof(buffer));
    if (bytes <= 0) {
        return false;
    }

    ssize_t offset = 0;
    while (offset < bytes) {
        struct inotify_event* event = reinterpret_cast<struct inotify_event*>(buffer + offset);
        if ((event->mask & expectedMask) != 0) {
            if (expectedName.empty()) {
                return true;
            }

            if (event->len > 0 && expectedName == std::string(event->name)) {
                return true;
            }
        }

        offset += sizeof(struct inotify_event) + event->len;
    }

    return false;
}
}

class Core_SprDirWatch : public ::testing::Test {
protected:
    void SetUp() override
    {
        char pathTemplate[] = "/tmp/spr_dir_watch_XXXXXX";
        char* created = mkdtemp(pathTemplate);
        ASSERT_TRUE(created != nullptr);
        mWatchDir = created;
    }

    void TearDown() override
    {
        if (!mCreatedFile.empty()) {
            remove(mCreatedFile.c_str());
            mCreatedFile.clear();
        }

        if (!mWatchDir.empty()) {
            rmdir(mWatchDir.c_str());
            mWatchDir.clear();
        }
    }

protected:
    std::string mWatchDir;
    std::string mCreatedFile;
};

// 测试添加目录监听并收到创建/删除事件
TEST_F(Core_SprDirWatch, AddWatchAndReceiveCreateDeleteEvents)
{
    SprDirWatch watcher;
    ASSERT_GE(watcher.GetInotifyFd(), 0);

    const uint32_t mask = IN_CREATE | IN_DELETE;
    int32_t wd = watcher.AddDirWatch(mWatchDir, mask);
    ASSERT_GE(wd, 0);

    const std::string fileName = "watched_file.txt";
    mCreatedFile = mWatchDir + "/" + fileName;

    {
        std::ofstream ofs(mCreatedFile);
        ofs << "watch-content";
    }
    EXPECT_TRUE(WaitForMaskEvent(watcher.GetInotifyFd(), IN_CREATE, fileName));

    ASSERT_EQ(0, remove(mCreatedFile.c_str()));
    mCreatedFile.clear();
    EXPECT_TRUE(WaitForMaskEvent(watcher.GetInotifyFd(), IN_DELETE, fileName));
}

// 测试添加无效路径监听返回失败
TEST_F(Core_SprDirWatch, AddWatchInvalidPathFails)
{
    SprDirWatch watcher;
    ASSERT_GE(watcher.GetInotifyFd(), 0);

    const std::string invalidPath = mWatchDir + "/not_exist_dir";
    EXPECT_EQ(-1, watcher.AddDirWatch(invalidPath, IN_CREATE));
}
