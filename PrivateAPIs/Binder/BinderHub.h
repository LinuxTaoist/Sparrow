/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BinderHub.h
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
#ifndef __BINDER_HUB_H__
#define __BINDER_HUB_H__

#include <thread>
#include <string>
#include <memory>
#include <stdint.h>
#include "Parcel.h"

class BinderHub
{
public:
    explicit BinderHub(const std::string& srvName);
    virtual ~BinderHub();

    int32_t InitializeHub();

    int32_t DestroyHub();
    void BinderLoop();
    virtual void handleCmd(const std::shared_ptr<Parcel>& pReqParcel, const std::shared_ptr<Parcel>& pRspParcel, int32_t cmd) = 0;

private:
    bool mRun;
    std::string mSrvName;
    std::thread mBindThread;
    std::shared_ptr<Parcel> mReqParcel;
    std::shared_ptr<Parcel> mRspParcel;
};

#endif // __BINDER_HUB_H__