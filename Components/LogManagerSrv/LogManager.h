/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LogManager.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__

#include "SprObserver.h"

class LogManager : public SprObserver
{
public:
    LogManager(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr);
    virtual ~LogManager();

    int ProcessMsg(const SprMsg& msg);
};

#endif
