/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Binder.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "Binder.h"

int32_t Binder::GetParcel(std::shared_ptr<Parcel>& reqParcel, std::shared_ptr<Parcel>& rspParcel)
{
    std::string rootPath = ""; ///tmp/";

    reqParcel = std::make_shared<Parcel>(rootPath + mName + "_req", mKey, true);
    rspParcel = std::make_shared<Parcel>(rootPath + mName + "_rsp", mKey, false);

    return 0;
}
