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

const int32_t MQ_BUFF_MAX_SIZE  = 1024;
const int32_t RANDOM_STR_LENGTH = 8;

// Module ID, Module Name, proxyRpc,
SprObserver::SprObserver(ModuleIDType id, const string& name, EProxyType proxyType)
    : mProxyType(proxyType), mModuleID(id), mModuleName(name), mMsgMediatorPtr(nullptr)
{
}

SprObserver::~SprObserver()
{
    SPR_LOGD("Exit Module: %s\n", mModuleName.c_str());
}

int32_t SprObserver::Initialize()
{
    SPR_LOGD("Initialize Module: [ID: %d Name: %s]\n", mModuleID, mModuleName.c_str());
    mMsgMediatorPtr = SprMediatorFactory::GetInstance()->GetMediatorProxy(mProxyType);
    if (!mMsgMediatorPtr) {
        SPR_LOGE("Failed to get mediator proxy! mProxyType = 0x%x\n", mProxyType);
        return -1;
    }

    DumpCommonVersion();
    InitFramework();     // Init framework module
    Init();              // Init business module
    return 0;
}

int32_t SprObserver::InitFramework()
{
    return 0;
}

int32_t SprObserver::Init()
{
    return 0;
}

int32_t SprObserver::NotifyObserver(SprMsg& msg)
{
    msg.SetFrom(mModuleID);
    return mMsgMediatorPtr->NotifyObserver(msg);
}

int32_t SprObserver::NotifyObserver(ModuleIDType id, SprMsg& msg)
{
    msg.SetFrom(mModuleID);
    msg.SetTo(id);
    return mMsgMediatorPtr->NotifyObserver(msg);
}

int32_t SprObserver::NotifyAllObserver(SprMsg& msg)
{
    // Notify all modules by setting destination
    // to MODULE_NONE, refer to SprMediator::NotifyAllObserver.
    msg.SetFrom(mModuleID);
    msg.SetTo(MODULE_NONE);
    return mMsgMediatorPtr->NotifyAllObserver(msg);
}

int32_t SprObserver::RegisterTimer(int32_t delayInMSec, int32_t intervalInMSec, uint32_t msgId, uint32_t repeatTimes)
{
    STimerInfo timeInfo = {mModuleID, msgId, repeatTimes, delayInMSec, intervalInMSec};
    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));

    SprMsg msg(MODULE_TIMERM, SIG_ID_TIMER_ADD_CUSTOM_TIMER);
    msg.SetDatas(pInfo, sizeof(STimerInfo));
    return NotifyObserver(msg);
}

int32_t SprObserver::UnregisterTimer(uint32_t msgId)
{
    STimerInfo timeInfo = {mModuleID, msgId, 0, 0, 0};
    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));

    SprMsg msg(MODULE_TIMERM, SIG_ID_TIMER_DEL_TIMER);
    msg.SetDatas(pInfo, sizeof(STimerInfo));
    return NotifyObserver(msg);
}

int32_t SprObserver::DumpCommonVersion()
{
    SPR_LOGD("- Dump common header version\n");
    SPR_LOGD("- CommonTypeDefs.h: %s\n", COMMON_TYPE_DEFS_VERSION);
    SPR_LOGD("- CommonMacros.h  : %s\n", COMMON_MACROS_VERSION);
    SPR_LOGD("- CoreTypeDefs.h  : %s\n", CORE_TYPE_DEFS_VERSION);
    return 0;
}
