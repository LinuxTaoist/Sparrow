/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : TimeManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/11/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/11/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "SprLog.h"
#include "TimeManager.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("TimeMgr", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGW("TimeMgr", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("TimeMgr", fmt, ##args)

#define DEFAULT_NTP_ADDR    "ntp.tencen.com"
#define DEFAULT_NTP_PORT    123

TimeManager::TimeManager(ModuleIDType id, const std::string& name)
            : SprObserverWithMQueue(id, name)
{
    mCurPriority = TIME_SOURCE_PRIORITY_BUTT;
    mCurTimeSource = TIME_SOURCE_TYPE_BUTT;
}

TimeManager::~TimeManager()
{
}

TimeManager* TimeManager::GetInstance(ModuleIDType id, const std::string& name)
{
    static TimeManager instance(id, name);
    return &instance;
}

int32_t TimeManager::InitDebugDetails()
{
    return 0;
}

int32_t TimeManager::RequestNtpTime()
{
    int ret = 0;
    std::string addr = DEFAULT_NTP_ADDR;
    uint16_t port = DEFAULT_NTP_PORT;
    mNtpCliPtr = make_shared<NtpClient>(addr, port, [&](double time) {
        SPR_LOGD("NTP time: %f", time);
    });

    ret = mNtpCliPtr->SendTimeRequest();
    return ret;
}

int32_t TimeManager::RequestGnssTime()
{
    return 0;
}

int32_t TimeManager::ProcessMsg(const SprMsg& msg)
{
    switch(msg.GetMsgId())
    {
        case SIG_ID_TIMEM_NTP_SYNC:
            RequestNtpTime();
            break;
        default:
            break;
    }

    return 0;
}
