/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BindingHub.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __BINDING_HUB_H__
#define __BINDING_HUB_H__

#include <map>
#include <string>
#include <stdint.h>
#include "BindCommon.h"

class BindingHub
{
public:
    ~BindingHub();

    static BindingHub* GetInstance();
    int32_t HandleMsgLoop();

private:
    BindingHub();

    int32_t EnvReady(const std::string& srvName);
    int32_t MsgResponseAddService();
    int32_t MsgResponseRemoveService();
    int32_t MsgResponseGetService();

private:
    using HandleFunction = int32_t (BindingHub::*)(void);

    std::map<std::string, BinderInfo> mBinderMap;
    std::map<int32_t, HandleFunction> mHandleFuncs;
};

#endif // __BINDING_HUB_H__
