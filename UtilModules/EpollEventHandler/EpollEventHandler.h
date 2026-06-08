/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : EpollEventHandler.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/05/07
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/05/07 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __EPOLL_EVENT_HANDLER_H__
#define __EPOLL_EVENT_HANDLER_H__

#include <map>
#include "IEpollEvent.h"

class EpollEventHandler
{
public:
    static EpollEventHandler* GetInstance(int32_t size = 0, int32_t blockTimeOut = -1);

    void AddPoll(IEpollEvent* p);
    void DelPoll(IEpollEvent* p);
    void EpollLoop();
    void ExitLoop();
    virtual void HandleEpollEvent(IEpollEvent& pEvent);

protected:
    explicit EpollEventHandler(int32_t size = 0, int32_t blockTimeOut = -1);
    virtual ~EpollEventHandler();

private:
    bool    mRun;
    int32_t     mHandle;
    int32_t     mTimeOut;
    std::map<int32_t, IEpollEvent*> mEpollMap;   // fd, type, IEpollEvent
};


#endif // __EPOLL_EVENT_HANDLER_H__
