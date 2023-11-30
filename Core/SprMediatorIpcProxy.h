/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMediatorIpcProxy.cpp
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
#include "SprMediatorProxy.h"

class SprMediatorIpcProxy : public SprMediatorProxy
{
public:
    SprMediatorIpcProxy();
    virtual ~SprMediatorIpcProxy();

    int RegisterObserver(const SprObserver& observer) override;
    int UnregisterObserver(const SprObserver& observer) override;
    int NotifyObserver() override;
};
