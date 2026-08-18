/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TestModuleThread.cc
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/08/18
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/08/18 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprLog.h"
#include "TestModuleThread.h"
#include "CoreTypeDefs.h"
#include "SprSigId.h"

using namespace InternalDefs;

#define LOG_TAG "TestModThd"

TestModuleThread::TestModuleThread()
    : SprObserverWithMQueueThread(InternalDefs::MODULE_GTEST_INTERNAL, "TestInternalThread"),
      mRecvCount(0),
      mSlowProcessingMs(0),
      mLastProcessThreadId(std::thread::id())
{
}

TestModuleThread::~TestModuleThread()
{
}

int32_t TestModuleThread::Init()
{
    return 0;
}

int32_t TestModuleThread::TriggerRecv()
{
    SprMsg msg;
    if (RecvMsg(msg) < 0) {
        SPR_LOGE("RecvMsg failed!\n");
        return -1;
    }

    return ProcessRecvMsg(msg);
}

int32_t TestModuleThread::ProcessMsg(const SprMsg& msg)
{
    SPR_LOGD("Recv msgId: %s\n", GetSigName(msg.GetMsgId()));

    mRecvCount++;
    mLastProcessThreadId = std::this_thread::get_id();

    // 模拟耗时业务处理，验证线程隔离：慢处理不应阻塞 epoll 循环
    int32_t slowMs = mSlowProcessingMs.load();
    if (slowMs > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(slowMs));
    }

    return 0;
}
