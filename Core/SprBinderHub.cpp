/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprBinderHub.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/17
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/17 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprLog.h"
#include "SprBinderHub.h"
#include "BindInterface.h"

#define SPR_LOGD(fmt, args...) LOGD("SprBinderHub", fmt, ##args)
#define SPR_LOGW(fmt, args...) LOGD("SprBinderHub", fmt, ##args)
#define SPR_LOGE(fmt, args...) LOGE("SprBinderHub", fmt, ##args)

SprBinderHub::SprBinderHub(const std::string& srvName) : mRun(false), mSrvName(srvName)
{
}

SprBinderHub::~SprBinderHub()
{
    DestoryHub();
}

int32_t SprBinderHub::InitializeHub()
{
    int32_t ret = -1;

    if (!mBindThread.joinable()) {
        mRun = true;
        mBindThread = std::thread(BinderLoop, this);
        ret = 0;
    }

    return ret;
}

int32_t SprBinderHub::DestoryHub()
{
    mRun = false;
    if (mBindThread.joinable())
    {
        mBindThread.join();
    }

    SPR_LOGE("Destory %s binderHub!\n", mSrvName.c_str());
    return 0;
}

void SprBinderHub::BinderLoop(void* pData)
{
    SprBinderHub* mSelf = (SprBinderHub*)pData;
    std::shared_ptr<Parcel> pReqParcel;
    std::shared_ptr<Parcel> pRspParcel;

    bool rs = BindInterface::GetInstance()->InitializeServiceBinder(mSelf->mSrvName, pReqParcel, pRspParcel);
    if (!rs)
    {
        SPR_LOGE("Binder init failed!\n");
        return;
    }

    SPR_LOGD("Start %s binder loop!\n", mSelf->mSrvName.c_str());
    do {
        int cmd = 0;
        pReqParcel->Wait();
        int ret = pReqParcel->ReadInt(cmd);
        if (ret != 0)
        {
            SPR_LOGE("ReadInt failed!\n");
            continue;
        }

        mSelf->handleCmd(pReqParcel, pRspParcel, cmd);
    } while(mSelf->mRun);

    SPR_LOGD("Exit %s binder loop!\n", mSelf->mSrvName.c_str());
}

