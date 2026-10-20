/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BinderHub.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/05/17
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "SprLog.h"
#include "BindCommon.h"
#include "AsyncEvent.h"
#include "CoreTypeDefs.h"
#include "CommonMacros.h"
#include "BindInterface.h"
#include "BinderHub.h"

using namespace InternalDefs;

#define LOG_TAG "BinderHub"

BinderHub::BinderHub(const std::string& srvName)
    : mRun(false), mSrvName(srvName) {
}

BinderHub::~BinderHub() {
    DestroyHub();
    if (mBindThread.joinable()) {
        mBindThread.join();
    }
}

int32_t BinderHub::InitializeHub() {
    int32_t ret = -1;
    if (!mBindThread.joinable()) {
        mRun = true;
        mBindThread = std::thread([this]() {
            BinderLoop();
        });
        ret = 0;
    }

    return ret;
}

int32_t BinderHub::DestroyHub() {
    if (mRun) {
        mRun = false;
        POINTER_CHECK_ERR(mReqParcel, -1);
        NONZERO_CHECK_RET(mReqParcel->WriteInt(GENERAL_CMD_EXE_EXIT));
        NONZERO_CHECK_RET(mReqParcel->Post());
    }
    return 0;
}

void BinderHub::BinderLoop() {
    bool rs = BindInterface::GetInstance()->InitializeServiceBinder(mSrvName, mReqParcel, mRspParcel);
    if (!rs) {
        SPR_LOGE("Binder init failed!");
        return;
    }

    SPR_LOGD("Start %s binder loop!", mSrvName.c_str());
    do {
        int32_t cmd = 0;
        int32_t ret = mReqParcel->Wait();
        if (ret != 0) {
            SPR_LOGE("Wait request failed! ret = %d", ret);
            break;
        }

        ret = mReqParcel->ReadInt(cmd);
        if (ret != 0) {
            SPR_LOGE("ReadInt failed!");
            continue;
        }

        if (cmd == GENERAL_CMD_EXE_EXIT) {
            mRun = false;
            SPR_LOGD("Binder loop exit!");
            break;
        } else if (cmd == GENERAL_REGISTER_CALLBACK || cmd == GENERAL_UNREGISTER_CALLBACK) {
            std::string channelName;
            ret = mReqParcel->ReadString(channelName);
            if (ret != 0) {
                SPR_LOGE("Read callback channel failed! ret = %d", ret);
                continue;
            }

            AsyncEvent* pEvent = AsyncEvent::GetInstance();
            if (!pEvent) {
                SPR_LOGE("pEvent is nullptr!");
                continue;
            }

            bool isRegister = (cmd == GENERAL_REGISTER_CALLBACK);
            ret = isRegister ? pEvent->AddWriter(channelName)
                             : pEvent->DelWriter(channelName);
            SPR_LOGD("%s callback %s, ret = %d",
                isRegister ? "Register" : "Unregister", channelName.c_str(), ret);

            mRspParcel->WriteInt(ret);
            mRspParcel->Post();
            continue;
        }

        handleCmd(mReqParcel, mRspParcel, cmd);
    } while(mRun);

    SPR_LOGD("Exit %s binder loop!", mSrvName.c_str());
}
