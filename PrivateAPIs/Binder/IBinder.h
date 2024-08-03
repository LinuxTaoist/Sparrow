/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : IBinder.h
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
#ifndef __IBINDER_H__
#define __IBINDER_H__

#include <memory>
#include <string>
#include "Parcel.h"

class IBinder
{
public:
    /**
     * @brief  Constructor
     *
     * @param name  service name
     * @param key  service key
     */
    IBinder(const std::string& name, int key) : mKey(key), mName(name) {};

    /**
     * @brief Destructor
     */
    ~IBinder() {};

    /**
     * @brief Get the Parcel object
     *
     * @param reqParcel  request parcel
     * @param rspParcel  response parcel
     * @return  0 if success, -1 if failed
     */
    int32_t GetParcel(std::shared_ptr<Parcel>& reqParcel, std::shared_ptr<Parcel>& rspParcel);

private:
    int mKey;
    std::string mName;
};

#endif // __IBINDER_H__
