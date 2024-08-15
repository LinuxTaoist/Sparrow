/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprObserver.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
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
#include "SprLog.h"
#include "CommonMacros.h"
#include "CommonTypeDefs.h"
#include "SprObserver.h"
#include "SprMediatorFactory.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...)  LOGD("SprObsBase", "[%s] " fmt, mModuleName.c_str(), ##args)
#define SPR_LOGE(fmt, args...)  LOGE("SprObsBase", "[%s] " fmt, mModuleName.c_str(), ##args)

const int MQ_BUFF_MAX_SIZE  = 1024;
const int RANDOM_STR_LENGTH = 8;

// Module ID, Module Name, proxyRpc,
SprObserver::SprObserver(ModuleIDType id, const string& name, EProxyType proxyType)
    : mProxyType(proxyType), mModuleID(id), mModuleName(name)
{
    mModuleID = id;
    mModuleName = name;
    mMsgMediatorPtr = SprMediatorFactory::GetInstance()->GetMediatorProxy(proxyType);
    mMsgMediatorPtr->RegisterObserver(*this);
    DumpCommonVersion();
    SPR_LOGD("Start Module: %d %s\n", id, mModuleName.c_str());
}

SprObserver::~SprObserver()
{
    mMsgMediatorPtr->UnregisterObserver(*this);
    SPR_LOGD("Exit Module: %s\n", mModuleName.c_str());
}

int SprObserver::NotifyObserver(SprMsg& msg)
{
    msg.SetFrom(mModuleID);
    mMsgMediatorPtr->NotifyObserver(msg);
    return 0;
}

int SprObserver::NotifyObserver(ModuleIDType id, SprMsg& msg)
{
    msg.SetFrom(mModuleID);
    msg.SetTo(id);
    mMsgMediatorPtr->NotifyObserver(msg);
    return 0;
}

int SprObserver::NotifyAllObserver(SprMsg& msg)
{
    // Notify all modules by setting destination
    // to MODULE_NONE, refer to SprMediator::NotifyAllObserver.
    msg.SetFrom(mModuleID);
    msg.SetTo(MODULE_NONE);
    mMsgMediatorPtr->NotifyAllObserver(msg);
    return 0;
}

int SprObserver::DumpCommonVersion()
{
    SPR_LOGD("- Dump common version: %s / %s / %s \n",
                COMMON_TYPE_DEFS_VERSION, COMMON_MACROS_VERSION, CORE_TYPE_DEFS_VERSION);
    return 0;
}

int SprObserver::MainLoop()
{
    return 0;
}

int SprObserver::MainExit()
{
    return 0;
}
