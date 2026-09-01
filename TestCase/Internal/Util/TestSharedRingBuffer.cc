/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestSharedRingBuffer.cc
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
#include <string>
#include <unistd.h>
#include "gtest/gtest.h"
#include "SharedRingBuffer.h"
#include "CommonMacros.h"

// 定义不同长度的 LOG_TAG
#define LOG_TAG "TestRingBuf"

// 帧协议头长度（int32_t 长度字段）
constexpr int32_t FRAME_HEAD = 4;

// 测试临时共享内存路径（与日志服务无关）
const std::string TMP_SHM_PATH = "/tmp/TestSprLogShm";

// 测试环形缓冲帧协议（[4B len][body] 多帧写入读取完整）
TEST(Util_SharedRingBuffer, FrameIntegrity) {
    unlink(TMP_SHM_PATH.c_str());

    SharedRingBuffer master(TMP_SHM_PATH, LOG_CACHE_MEMORY_SIZE);
    ASSERT_GE(master.AvailSpace(), 0) << "create master failed";

    const int32_t FRAME_NUM = 100;
    for (int32_t i = 0; i < FRAME_NUM; ++i) {
        std::string body = "direct-frame-" + std::to_string(i);
        std::vector<char> frame(FRAME_HEAD + body.size());
        int32_t len = (int32_t)body.size();
        memcpy(frame.data(), &len, FRAME_HEAD);
        memcpy(frame.data() + FRAME_HEAD, body.data(), body.size());
        EXPECT_EQ(master.Write(frame.data(), (int32_t)frame.size()), 0);
    }

    // 读端逐帧校验
    SharedRingBuffer slave(TMP_SHM_PATH);
    for (int32_t i = 0; i < FRAME_NUM; ++i) {
        int32_t len = 0;
        ASSERT_EQ(slave.Read(&len, FRAME_HEAD), 0) << "read header failed " << i;
        ASSERT_GT(len, 0);
        std::vector<char> body(len);
        ASSERT_EQ(slave.Read(body.data(), len), 0) << "read body failed " << i;
        EXPECT_EQ(std::string(body.data(), len), "direct-frame-" + std::to_string(i));
    }

    unlink(TMP_SHM_PATH.c_str());
}

// 测试缓冲写满行为（写满后不阻塞，读端消费后可恢复）
TEST(Util_SharedRingBuffer, OverflowRecovery) {
    unlink(TMP_SHM_PATH.c_str());

    SharedRingBuffer master(TMP_SHM_PATH, LOG_CACHE_MEMORY_SIZE);
    ASSERT_GE(master.AvailSpace(), 0) << "create master failed";

    // 用 1MB 块快速填满约 10MB 缓冲
    std::vector<char> chunk(1024 * 1024, 'x');
    int32_t wrote = 0;
    while (wrote < 20) {
        if (master.Write(chunk.data(), (int32_t)chunk.size()) != 0) {
            break;
        }
        wrote++;
    }
    // 容量 10MB，应能写入约 9~10 块后失败（有界，不无限写）
    EXPECT_GE(wrote, 8);
    EXPECT_LT(wrote, 20);

    // 写满后再写应失败（不阻塞、不崩溃）
    EXPECT_NE(master.Write(chunk.data(), (int32_t)chunk.size()), 0);

    // 读端消费全部数据后恢复可写
    SharedRingBuffer slave(TMP_SHM_PATH);
    int32_t availData = slave.AvailData();
    while (availData > 0) {
        std::vector<char> tmp(std::min(availData, 1024 * 1024));
        slave.Read(tmp.data(), (int32_t)tmp.size());
        availData = slave.AvailData();
    }
    EXPECT_EQ(master.Write(chunk.data(), (int32_t)chunk.size()), 0);

    unlink(TMP_SHM_PATH.c_str());
}
