/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverWithMQueueThread.cpp
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
#include "CommonErrorCodes.h"
#include "SprObserverWithMQueueThread.h"
 
using namespace InternalDefs;

#define LOG_TAG "SprObsThread"

#define DEFAULT_QUEUE_MAX_SIZE 1024

SprObserverWithMQueueThread::SprObserverWithMQueueThread(ModuleIDType id, const std::string& name, EProxyType proxyType)
    : SprObserverWithMQueue(id, name, proxyType),
      mRunning(false),
      mMaxQueueSize(DEFAULT_QUEUE_MAX_SIZE)
{
    mRunning.store(true);
    mThread = std::thread(&SprObserverWithMQueueThread::ThreadLoop, this);
    SPR_LOGD("Processing thread started!\n");
}

SprObserverWithMQueueThread::~SprObserverWithMQueueThread()
{
    StopThread();
}

int32_t SprObserverWithMQueueThread::ProcessRecvMsg(SprMsg& msg)
{
    std::lock_guard<std::mutex> lock(mQueueMutex);
    if (mMsgQueue.size() >= mMaxQueueSize) {
        SPR_LOGE("Message queue full (max = %u), drop msg! msgId = %u\n", mMaxQueueSize, msg.GetMsgId());
        SendEventToMonitor(ERR_GENERAL_RUN_LONGTIME, "observer thread queue full, msg dropped");
        return -1;
    }

    mMsgQueue.push(msg);
    mQueueCond.notify_one();
    return 0;
}

void SprObserverWithMQueueThread::ThreadLoop()
{
    SPR_LOGD("Processing thread running!\n");

    while (mRunning.load()) {
        SprMsg msg;
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mQueueCond.wait(lock, [this] {
                return !mRunning.load() || !mMsgQueue.empty();
            });

            if (!mRunning.load() && mMsgQueue.empty()) {
                break;
            }

            msg = std::move(mMsgQueue.front());
            mMsgQueue.pop();
        }

        DispatchSprMsg(msg);
    }

    SPR_LOGD("Processing thread exit!\n");
}

void SprObserverWithMQueueThread::StopThread()
{
    if (!mRunning.load()) {
        return;
    }

    mRunning.store(false);
    mQueueCond.notify_all();

    if (mThread.joinable()) {
        mThread.join();
    }

    SPR_LOGD("Processing thread stopped!\n");
}
