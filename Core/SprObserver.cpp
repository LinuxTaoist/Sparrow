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

#define LOG_TAG "SprObsBase"

const int32_t MQ_BUFF_MAX_SIZE  = 1024;
const int32_t RANDOM_STR_LENGTH = 8;

// Module ID, Module Name, proxyRpc,
SprObserver::SprObserver(ModuleIDType id, const string& name, EProxyType proxyType)
    : mProxyType(proxyType), mModuleID(id), mModuleName(name) {
}

SprObserver::~SprObserver() {
    SPR_LOGD("Exit Module: %s", mModuleName.c_str());
}

int32_t SprObserver::Initialize() {
    SPR_LOGD("Initialize Module: [ID: %d Name: %s]", mModuleID, mModuleName.c_str());
    DumpCommonVersion();
    InitFramework();     // Init module's framework
    Init();              // Init module's business
    return 0;
}

int32_t SprObserver::Deinitialize() {
    Deinit();            // Deinit module's business
    DeinitFramework();   // Deinit module's framework
    return 0;
}

int32_t SprObserver::InitFramework() {
    return 0;
}

int32_t SprObserver::DeinitFramework() {
    return 0;
}

int32_t SprObserver::Deinit() {
    return 0;
}

SprMediatorProxy* SprObserver::GetMediatorProxy(InternalDefs::EProxyType type) {
    SprMediatorFactory* pFactory = SprMediatorFactory::GetInstance();
    if (!pFactory) {
        SPR_LOGE("pFactory is nullptr!");
        return nullptr;
    }

    SprMediatorProxy* pMediatorProxy = pFactory->GetMediatorProxy(type);
    if (!pMediatorProxy) {
        SPR_LOGE("pMediatorProxy is nullptr! mProxyType = 0x%x", type);
        return nullptr;
    }

    return pMediatorProxy;
}

int32_t SprObserver::NotifyObserver(SprMsg& msg) {
    SprMediatorProxy* pMediatorProxy = GetMediatorProxy(mProxyType);
    if (!pMediatorProxy) {
        return -1;
    }

    msg.SetFrom(mModuleID);
    return pMediatorProxy->NotifyObserver(msg);
}

int32_t SprObserver::NotifyObserver(uint32_t id, SprMsg& msg) {
    SprMediatorProxy* pMediatorProxy = GetMediatorProxy(mProxyType);
    if (!pMediatorProxy) {
        return -1;
    }

    msg.SetFrom(mModuleID);
    msg.SetTo(id);
    return pMediatorProxy->NotifyObserver(msg);
}

int32_t SprObserver::NotifyAllObserver(SprMsg& msg) {
    SprMediatorProxy* pMediatorProxy = GetMediatorProxy(mProxyType);
    if (!pMediatorProxy) {
        return -1;
    }

    // Notify all modules by setting destination
    // to MODULE_NONE, refer to SprMediator::NotifyAllObserver.
    msg.SetFrom(mModuleID);
    msg.SetTo(MODULE_NONE);
    return pMediatorProxy->NotifyAllObserver(msg);
}

int32_t SprObserver::RegisterTimer(int32_t delayInMSec, int32_t intervalInMSec, uint32_t msgId, uint32_t repeatTimes) {
    STimerInfo timeInfo = {mModuleID, msgId, repeatTimes, delayInMSec, intervalInMSec};
    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));

    SprMsg msg(MODULE_TIMERM, SIG_ID_TIMER_ADD_CUSTOM_TIMER);
    msg.SetDatas(pInfo, sizeof(STimerInfo));
    return NotifyObserver(msg);
}

int32_t SprObserver::UnregisterTimer(uint32_t msgId) {
    STimerInfo timeInfo = {mModuleID, msgId, 0, 0, 0};
    shared_ptr<STimerInfo> pInfo = static_pointer_cast<STimerInfo>(make_shared<STimerInfo>(timeInfo));

    SprMsg msg(MODULE_TIMERM, SIG_ID_TIMER_DEL_CUSTOM_TIMER);
    msg.SetDatas(pInfo, sizeof(STimerInfo));
    return NotifyObserver(msg);
}

int32_t SprObserver::DumpCommonVersion() {
    SPR_LOGD("- Dump common header version");
    SPR_LOGD("- CommonTypeDefs.h: %s", COMMON_TYPE_DEFS_VERSION);
    SPR_LOGD("- CommonMacros.h  : %s", COMMON_MACROS_VERSION);
    SPR_LOGD("- CoreTypeDefs.h  : %s", CORE_TYPE_DEFS_VERSION);
    return 0;
}

int32_t SprObserver::SetPrintLogLevel(int32_t level) {
    int32_t curLevel = SprLog::GetInstance()->GetLevel();
    if (level < LOG_LEVEL_MIN || level > LOG_LEVEL_BUTT) {
        SPR_LOGE("Invalid log level: %d", level);
        return -1;
    }

    if (level != curLevel) {
        SprLog::GetInstance()->SetLevel(level);
        SPR_LOGD("Set log level %d -> %d", curLevel, level);
    }
    return 0;
}

int32_t SprObserver::SetPrintLogLength(int32_t length) {
    if (length < 0) {
        SPR_LOGE("Invalid log length: %d", length);
        return -1;
    }

    int32_t curLength = SprLog::GetInstance()->GetLength();
    if (length != curLength) {
        SprLog::GetInstance()->SetLength(length);
        SPR_LOGD("Set log length %d -> %d", curLength, length);
    }
    return 0;
}
