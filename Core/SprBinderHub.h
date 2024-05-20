/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprBinderHub.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
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
#ifndef __SPR_BINDER_HUB_H__
#define __SPR_BINDER_HUB_H__

#include <thread>
#include <string>
#include <memory>
#include "Parcel.h"

class SprBinderHub
{
public:
    SprBinderHub(const std::string& srvName);
    virtual ~SprBinderHub();

    bool InitializeHub();

    static void BinderLoop(void* pData);
    virtual void handleCmd(std::shared_ptr<Parcel> pReqParcel, std::shared_ptr<Parcel> pRspParcel, int cmd) = 0;

private:
    bool mRun;
    std::string mSrvName;
    std::thread mBindThread;
};

#endif // __SPR_BINDER_HUB_H__
