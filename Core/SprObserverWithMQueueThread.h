/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserverWithMQueueThread.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/08/18
 *
 *  Threaded message processing observer based on SprObserverWithMQueue. Incoming messages are buffered
 *  into an internal queue and processed by a dedicated thread, so slow business processing never blocks
 *  other observers running on the shared epoll loop.
 *
 *  Interface is identical to SprObserverWithMQueue, components can switch between them by only changing
 *  the base class name.
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/08/18 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SPR_OBSERVER_WITH_MQUEUE_THREAD_H__
#define __SPR_OBSERVER_WITH_MQUEUE_THREAD_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

#include "SprMsg.h"
#include "SprObserverWithMQueue.h"

class SprObserverWithMQueueThread : public SprObserverWithMQueue
{
public:
    /**
     * @brief  Construct / Destruct
     *
     * @param id module id
     * @param name module name
     * @param proxyType message proxy type
     */
    SprObserverWithMQueueThread(ModuleIDType id, const std::string& name, InternalDefs::EProxyType proxyType = InternalDefs::MEDIATOR_PROXY_MQUEUE);
    virtual ~SprObserverWithMQueueThread();

protected:
    /**
     * @brief  Enqueue received message and notify the processing thread
     *
     * @param msg received message
     * @return 0 on success, or -1 if the message is dropped due to a full queue
     */
    virtual int32_t ProcessRecvMsg(SprMsg& msg) final override;

private:
    /**
     * @brief  Processing thread entry
     *
     * Dequeue messages in FIFO order and dispatch them via DispatchSprMsg().
     */
    void ThreadLoop();

    /**
     * @brief  Stop the processing thread and wait for it to exit
     */
    void StopThread();

private:
    std::queue<SprMsg>        mMsgQueue;      // Internal message queue (FIFO)
    std::mutex                mQueueMutex;    // Guards mMsgQueue
    std::condition_variable   mQueueCond;     // Notifies the processing thread
    std::thread               mThread;        // Dedicated processing thread
    std::atomic<bool>         mRunning;       // Thread life-cycle flag
    uint32_t                  mMaxQueueSize;  // Queue capacity for back-pressure
};

#endif // __SPR_OBSERVER_WITH_MQUEUE_THREAD_H__
