/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IEpollEvent.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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

#ifndef __IEPOLL_EVENT_H__
#define __IEPOLL_EVENT_H__

#include <string>
#include <stdio.h>
#include "EpollDefs.h"

class IEpollEvent
{
public:
    IEpollEvent(int fd, EpollType eType = EPOLL_TYPE_BEGIN, void* arg = nullptr)
        : mEpollFd(fd), mEpollType(eType), mArgs(arg) {};

    virtual ~IEpollEvent() = default;
    virtual ssize_t Write(int fd, const std::string& bytes);
    virtual ssize_t Read(int fd, std::string& bytes);
    virtual void*   EpollEvent(int fd, EpollType eType, void* arg) = 0;

    int         GetEpollFd()        { return mEpollFd; }
    EpollType   GetEpollType()      { return mEpollType; }
    void*       GetArgs()           { return mArgs; }

protected:
    int         mEpollFd;
    EpollType   mEpollType;
    void*       mArgs;
};

#endif // __IEPOLL_EVENT_H__
