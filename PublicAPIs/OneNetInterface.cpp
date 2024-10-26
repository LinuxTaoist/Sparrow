/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetInterface.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/24
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/24 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <unistd.h>
#include <sys/types.h>
#include "Parcel.h"
#include "CoreTypeDefs.h"
#include "GeneralUtils.h"
#include "BindInterface.h"
#include "OneNetInterface.h"

using namespace InternalDefs;
using namespace GeneralUtils;

#define SPR_LOGI(fmt, args...) printf("%s %6d %12s I: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IOneNet", __LINE__, ##args)
#define SPR_LOGD(fmt, args...) printf("%s %6d %12s D: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IOneNet", __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%s %6d %12s W: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IOneNet", __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%s %6d %12s E: %4d " fmt, GetCurTimeStr().c_str(), getpid(), "IOneNet", __LINE__, ##args)

static bool mEnable;
std::shared_ptr<Parcel> pReqParcel = nullptr;
std::shared_ptr<Parcel> pRspParcel = nullptr;

OneNetInterface::OneNetInterface()
{
    mEnable = true;
    bool ret = BindInterface::GetInstance()->InitializeClientBinder("OneNetMqtt", pReqParcel, pRspParcel);
    if (!ret || !pReqParcel || !pRspParcel) {
        mEnable = false;
    }
}

OneNetInterface::~OneNetInterface()
{
    mEnable = false;
}

OneNetInterface* OneNetInterface::GetInstance()
{
    static OneNetInterface instance;
    return &instance;
}

bool OneNetInterface::IsReady()
{
    return mEnable;
}

int OneNetInterface::ActiveDevice(const std::string& deviceName)
{
    if (!mEnable) {
        SPR_LOGE("OneNet is disable!\n");
        return -1;
    }

    pReqParcel->WriteInt(ONENET_CMD_ACTIVE_DEVICE);
    pReqParcel->WriteString(deviceName);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}

int OneNetInterface::DeactiveDevice()
{
    if (!mEnable) {
        SPR_LOGE("OneNet is disable!\n");
        return -1;
    }

    pReqParcel->WriteInt(ONENET_CMD_DEACTIVE_DEVICE);
    pReqParcel->Post();

    int ret = 0;
    pRspParcel->Wait();
    pRspParcel->ReadInt(ret);

    SPR_LOGD("ret: %d\n", ret);
    return ret;
}
