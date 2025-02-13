/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : EpollDefs.h
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

#ifndef __EPOLL_DEFS_H__
#define __EPOLL_DEFS_H__

enum EpollType
{
    EPOLL_TYPE_BEGIN,
    EPOLL_TYPE_FILE,
    EPOLL_TYPE_SOCKET,
    EPOLL_TYPE_UART,
    EPOLL_TYPE_USB,
    EPOLL_TYPE_TIMERFD,
    EPOLL_TYPE_SIGNAL,
    EPOLL_TYPE_EVENT,
    EPOLL_TYPE_MQUEUE,
    EPOLL_TYPE_PIPE,
    EPOLL_TYPE_END
};

#endif // __EPOLL_DEFS_H__
