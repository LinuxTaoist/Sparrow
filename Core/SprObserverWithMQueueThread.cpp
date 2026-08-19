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
#define DEFAULT_WAIT_TIMEOUT_MS 100

SprObserverWithMQueueThread::SprObserverWithMQueueThread(ModuleIDType id, const std::string& name, EProxyType proxyType)
    : SprObserverWithMQueue(id, name, proxyType),
      mRunning(false),
      mMaxQueueSize(DEFAULT_QUEUE_MAX_SIZE) {
    StartThread();
}

SprObserverWithMQueueThread::~SprObserverWithMQueueThread() {
    StopThread();
}

int32_t SprObserverWithMQueueThread::ProcessRecvMsg(SprMsg& msg) {
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

void SprObserverWithMQueueThread::ThreadLoop() {
    SPR_LOGD("Enter MQ thread!");
    while (mRunning.load()) {
        SprMsg msg;
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            const auto deadline = std::chrono::steady_clock::now()
                                + std::chrono::milliseconds(DEFAULT_WAIT_TIMEOUT_MS);
            mQueueCond.wait_until(lock, deadline);
            if (!mRunning.load()) {
                break;
            }

            if (mMsgQueue.empty()) {
                continue;
            }

            msg = std::move(mMsgQueue.front());
            mMsgQueue.pop();
        }

        DispatchSprMsg(msg);
    }

    SPR_LOGD("Exit MQ thread!");
}

void SprObserverWithMQueueThread::StartThread() {
    mRunning.store(true);
    if (!mThread.joinable()) {
        mThread = std::thread(&SprObserverWithMQueueThread::ThreadLoop, this);
    }

    SPR_LOGD("Start MQ thread!");
}

void SprObserverWithMQueueThread::StopThread() {
    {
        std::lock_guard<std::mutex> lock(mQueueMutex);
        std::queue<SprMsg> empty;
        std::swap(mMsgQueue, empty);
    }

    mRunning.store(false);
    mQueueCond.notify_all();
    if (mThread.joinable()) {
        mThread.join();
    }

    SPR_LOGD("Stop MQ thread!");
}
