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
#include "BindCommon.h"
#include "SprBinderHub.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "BindInterface.h"
#include "AsyncEvent.h"

using namespace InternalDefs;

#define LOG_TAG "SprBinderHub"

bool SprBinderHub::mRun = false;

SprBinderHub::SprBinderHub(const std::string& srvName) : mSrvName(srvName)
{
}

SprBinderHub::~SprBinderHub()
{
    DestoryHub();
    if (mBindThread.joinable()) {
        mBindThread.join();
    }
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
    if (mRun) {
        mRun = false;
        POINTER_CHECK_ERR(mReqParcel, -1);
        NONZERO_CHECK_RET(mReqParcel->WriteInt(GENERAL_CMD_EXE_EXIT));
        NONZERO_CHECK_RET(mReqParcel->Post());
    }
    return 0;
}

void SprBinderHub::BinderLoop(void* pData)
{
    SprBinderHub* mSelf = reinterpret_cast<SprBinderHub*>(pData);
    bool rs = BindInterface::GetInstance()->InitializeServiceBinder(mSelf->mSrvName, mSelf->mReqParcel, mSelf->mRspParcel);
    if (!rs) {
        SPR_LOGE("Binder init failed!\n");
        return;
    }

    SPR_LOGD("Start %s binder loop!\n", mSelf->mSrvName.c_str());
    do {
        int32_t cmd = 0;
        mSelf->mReqParcel->Wait();
        int32_t ret = mSelf->mReqParcel->ReadInt(cmd);
        if (ret != 0) {
            SPR_LOGE("ReadInt failed!\n");
            continue;
        }

        if (cmd == GENERAL_CMD_EXE_EXIT) {
            mRun = false;
            SPR_LOGD("Binder loop exit!\n");
            break;
        } else if (cmd == GENERAL_REGISTER_CALLBACK) {
            std::string name;
            ret = mSelf->mReqParcel->ReadString(name);

            int32_t rc = -1;
            if (ret == 0) {
                rc = AsyncEvent::GetInstance()->AsWriter(name);
                SPR_LOGD("Register callback %s, ret = %d\n", name.c_str(), ret);
            }

            mSelf->mRspParcel->WriteInt(rc);
            mSelf->mRspParcel->Post();
        }

        mSelf->handleCmd(mSelf->mReqParcel, mSelf->mRspParcel, cmd);
    } while(mRun);

    SPR_LOGD("Exit %s binder loop!\n", mSelf->mSrvName.c_str());
}
