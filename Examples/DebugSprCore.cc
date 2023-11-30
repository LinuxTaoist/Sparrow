/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugSprCore.cc
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
#include <stdio.h>
#include <memory>
#include "SprObserver.h"
#include "SprMediatorIpcProxy.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%d DebugCore D: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d DebugCore E: " fmt, __LINE__, ##args)

class DebugCore : public SprObserver
{
public:
    DebugCore(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr)
        : SprObserver(id, name, mMsgMediatorPtr)
    {
    }

    ~DebugCore()
    {
    }

    int ProcessMsg(const SprMsg& msg)
    {
        return 0;
    }
};

int main(int agrc, const char *argv[])
{
    DebugCore theDebug(ESprModuleID::MODULE_LOGM, "Debug", make_shared<SprMediatorIpcProxy>());
    return 0;
}

