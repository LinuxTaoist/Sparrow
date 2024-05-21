/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : Binder.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/03/16
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/03/16 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __BINDER_H__
#define __BINDER_H__

#include <memory>
#include <string>
#include <stdint.h>
#include "Parcel.h"

class Binder
{
public:
    Binder(const std::string& name, int key) : mKey(key), mName(name) {};
    ~Binder() {};

    int32_t GetParcel(std::shared_ptr<Parcel>& reqParcel, std::shared_ptr<Parcel>& rspParcel);

private:
    int32_t mKey;
    std::string mName;
};

#endif // __BINDER_H__

