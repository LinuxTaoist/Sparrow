/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IBinder.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/16
 *---------------------------------------------------------------------------------------------------------------------
 */
#include "IBinder.h"

int32_t IBinder::GetParcel(std::shared_ptr<Parcel>& reqParcel, std::shared_ptr<Parcel>& rspParcel) {
    reqParcel = std::make_shared<Parcel>(mName + "_req", mKey, false);
    rspParcel = std::make_shared<Parcel>(mName + "_rsp", mKey, false);

    return 0;
}