/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : DebugModuleHub.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/28
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <memory>
#include "SprLog.h"
#include "CoreTypeDefs.h"
#include "DebugModuleHub.h"

using namespace std;
using namespace InternalDefs;

#define SPR_LOGD(fmt, args...) LOGD("DebugModule", fmt, ##args)
#define SPR_LOGI(fmt, args...) LOGI("DebugModule", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGD("DebugModule", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("DebugModule", fmt, ##args)

DebugModuleHub::DebugModuleHub(const std::string& srvName, DebugModule* pDebugModule) : SprBinderHub(srvName)
{
    mpDebugModule = pDebugModule;
}

DebugModuleHub::~DebugModuleHub()
{

}

void DebugModuleHub::handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int cmd)
{
    switch(cmd) {
        case DEBUG_CMD_ENABLE_REMOTE_PORT: {
            SprMsg msg(SIG_ID_DEBUG_ENABLE_REMOTE_SHELL);
            mpDebugModule->SendMsg(msg);
            pRspParcel->WriteInt(0);
            pRspParcel->Post();
            break;
        }
        case DEBUG_CMD_DISABLE_REMOTE_PORT: {
            SprMsg msg(SIG_ID_DEBUG_DISABLE_REMOTE_SHELL);
            mpDebugModule->SendMsg(msg);
            pRspParcel->WriteInt(0);
            pRspParcel->Post();
            break;
        }
        case DEBUG_CMD_TEST_ADD_1S_TIMER: {
            STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST_1S, 0, 0, 1000};
            auto pInfo = make_shared<STimerInfo>(timeInfo);
            SprMsg msg(MODULE_DEBUG, SIG_ID_TIMER_ADD_CUSTOM_TIMER);
            msg.SetDatas(pInfo, sizeof(STimerInfo));
            mpDebugModule->SendMsg(msg);

            pRspParcel->WriteInt(0);
            pRspParcel->Post();
            break;
        }
        case DEBUG_CMD_TEST_DEL_1S_TIMER: {
            STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST_1S, 0, 0, 0};
            auto pInfo = make_shared<STimerInfo>(timeInfo);
            SprMsg msg(MODULE_DEBUG, SIG_ID_TIMER_DEL_TIMER);
            msg.SetDatas(pInfo, sizeof(STimerInfo));
            mpDebugModule->SendMsg(msg);

            pRspParcel->WriteInt(0);
            pRspParcel->Post();
            break;
        }
        case DEBUG_CMD_TEST_ADD_CUSTOM_TIMER: {
            int32_t repeatTimes;
            int32_t delayInMilliSec;
            int32_t intervalInMilliSec;

            pReqParcel->ReadInt(repeatTimes);
            pReqParcel->ReadInt(delayInMilliSec);
            pReqParcel->ReadInt(intervalInMilliSec);

            STimerInfo timeInfo = { MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST,
                                   (uint32_t)repeatTimes, delayInMilliSec, intervalInMilliSec};
            auto pInfo = make_shared<STimerInfo>(timeInfo);
            SprMsg msg(MODULE_DEBUG, SIG_ID_TIMER_ADD_CUSTOM_TIMER);
            msg.SetDatas(pInfo, sizeof(STimerInfo));
            mpDebugModule->SendMsg(msg);

            pRspParcel->WriteInt(0);
            pRspParcel->Post();
            break;
        }
        case DEBUG_CMD_TEST_DEL_CUSTOM_TIMER: {
            STimerInfo timeInfo = {MODULE_DEBUG, SIG_ID_DEBUG_TIMER_TEST, 0, 0, 0};
            auto pInfo = make_shared<STimerInfo>(timeInfo);
            SprMsg msg(MODULE_DEBUG, SIG_ID_TIMER_DEL_TIMER);
            msg.SetDatas(pInfo, sizeof(STimerInfo));
            mpDebugModule->SendMsg(msg);

            pRspParcel->WriteInt(0);
            pRspParcel->Post();
            break;
        }
        case DEBUG_CMD_ENABLE_REMOTE_SHELL: {
            SprMsg msg(MODULE_DEBUG, SIG_ID_DEBUG_ENABLE_REMOTE_SHELL);
            mpDebugModule->SendMsg(msg);
            pRspParcel->WriteInt(0);
            pRspParcel->Post();
            break;
        }
        case DEBUG_CMD_DISABLE_REMOTE_SHELL: {
            SprMsg msg(MODULE_DEBUG, SIG_ID_DEBUG_DISABLE_REMOTE_SHELL);
            mpDebugModule->SendMsg(msg);
            pRspParcel->WriteInt(0);
            pRspParcel->Post();
            break;
        }
        default: {
            SPR_LOGW("Unknown cmd: 0x%x\n", cmd);
            break;
        }
    }
}
