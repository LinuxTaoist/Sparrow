/**
 *------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManager.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2023/12/21
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *------------------------------------------------------------------------------
 *  2023/12/21 | 1.0.0.1   | Xiang.D        | Create file
 *------------------------------------------------------------------------------
 *
 */
#include "PowerManager.h"

using namespace std;
using namespace SprPower;
using namespace InternalEnum;

#define SPR_LOGD(fmt, args...) printf("%d SprPower D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d SprPower W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SprPower E: " fmt, __LINE__, ##args)

vector<StateTransition <EPowerLev1State, EPowerLev2State, ESprSigId, PowerManager, SprMsg>>
PowerManager::mStateTable =
{

};

PowerManager::PowerManager(ModuleIDType id, const std::string& name, std::shared_ptr<SprMediatorProxy> mMsgMediatorPtr)
            : SprObserver(id, name, mMsgMediatorPtr)
{

}

PowerManager::~PowerManager()
{

}

int PowerManager::ProcessMsg(const SprMsg& msg)
{
    // loop: 遍历状态表，进入与状态匹配的入口
    for (const auto& it : mStateTable)
    {
        if (   (   (it.lev1State  == mCurLev1State)
                || (it.lev1State  == LEV1_POWER_ANY)
               )
            && (   (it.lev2State  == mCurLev2State)
                || (it.lev2State  == LEV2_POWER_ANY)
               )
            && (   (it.sigId      == msg.GetMsgId())
                || (it.sigId      == SIG_ID_ANY)
               )
           )
        {
            (this->*(it.callback))(msg);
            break;
        }
    }

    return 0;
}