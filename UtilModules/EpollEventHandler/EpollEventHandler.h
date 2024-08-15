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
    virtual ~EpollEventHandler();
    static EpollEventHandler* GetInstance(int size = 0, int blockTimeOut = -1);

    void AddPoll(IEpollEvent* p);
    void DelPoll(IEpollEvent* p);
    void EpollLoop(bool bRun);
    void ExitLoop();
    virtual void HandleEpollEvent(IEpollEvent& pEvent);

protected:
    explicit EpollEventHandler(int size = 0, int blockTimeOut = -1);

private:
    bool    mRun;
    int     mHandle;
    int     mTimeOut;
    std::map<int, IEpollEvent*> mEpollMap;   // fd, type, IEpollEvent
};


#endif // __EPOLL_EVENT_HANDLER_H__
