/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BinderHub.cpp
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
#include "BinderHub.h"
#include "CommonMacros.h"
#include "CoreTypeDefs.h"
#include "BindInterface.h"
#include "AsyncEvent.h"

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
        SPR_LOGE("Binder init failed!\n");
        return;
    }

    SPR_LOGD("Start %s binder loop!\n", mSrvName.c_str());
    do {
        int32_t cmd = 0;
        mReqParcel->Wait();
        int32_t ret = mReqParcel->ReadInt(cmd);
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
            ret = mReqParcel->ReadString(name);

            int32_t rc = -1;
            if (ret == 0) {
                rc = AsyncEvent::GetInstance()->AsWriter(name);
                SPR_LOGD("Register callback %s, ret = %d\n", name.c_str(), ret);
            }

            mRspParcel->WriteInt(rc);
            mRspParcel->Post();
        }

        handleCmd(mReqParcel, mRspParcel, cmd);
    } while(mRun);

    SPR_LOGD("Exit %s binder loop!\n", mSrvName.c_str());
}
