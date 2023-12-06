/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprEpollSchedule.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#ifndef SPR_EPOLL_SCHEDULE_H__
#define SPR_EPOLL_SCHEDULE_H__

#include <memory>
#include "SprMsg.h"
#include "SprObserver.h"
#include "libgo/libgo.h"

class SprEpollSchedule
{
public:
    using SprMsgHandler = void (*)(void *);

    ~SprEpollSchedule();
    /**
     * @brief       单例
     */
    static SprEpollSchedule* GetInstance();

    /**
     * @brief       初始化
     */
    void Init();

    /**
     * @brief       退出
     */
    void Exit();

    /**
     * @brief       添加监听事件
     *
     * @param[in]   observer
     */
    void AddPoll(SprObserver& observer);

    /**
     * @brief       添加监听事件
     *
     * @param[in]   handler 句柄
     */
    void AddPoll(int handler, void * pHandle);

    /**
     * @brief       删除监听事件
     *
     * @param[in]   observer
     */
    void DelPoll(SprObserver& observer);

    /**
     * @brief       开始监听
     */
    void StartEpoll();

private:
    bool mQuit;
    int mEpollFd;
    co::AsyncCoroutinePool* mpGoPool;

    /**
     * @brief       构造
     *
     * @param[in]   size 最大监听数目
     */
    explicit SprEpollSchedule(uint32_t size);
};

#endif
