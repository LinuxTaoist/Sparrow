/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : LogManager.cpp
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
#include "LogManager.h"

using namespace std;

LogManager::LogManager(ModuleIDType id, const std::string& name, shared_ptr<SprMediatorProxy> mMsgMediatorPtr)
            : SprObserver(id, name, mMsgMediatorPtr)
{

}

LogManager::~LogManager()
{

}

int LogManager::ProcessMsg(const SprMsg& msg)
{
    return 0;
}

