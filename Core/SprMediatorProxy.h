/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorProxy.h
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
#ifndef __SPR_MEDIATOR_PROXY_H__
#define __SPR_MEDIATOR_PROXY_H__

class SprObserver;
class SprMediatorProxy
{
public:
    SprMediatorProxy() {}
    virtual ~SprMediatorProxy() {}
    SprMediatorProxy(const SprMediatorProxy&) = delete;
    SprMediatorProxy& operator=(SprMediatorProxy&) = delete;
    SprMediatorProxy(SprMediatorProxy&&) = delete;
    SprMediatorProxy& operator=(SprMediatorProxy&&) = delete;

    virtual int RegisterObserver(const SprObserver& observer) = 0;
    virtual int UnregisterObserver(const SprObserver& observer) = 0;
    virtual int NotifyObserver() = 0;
};

#endif
