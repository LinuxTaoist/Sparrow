/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetDriver.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/08/13
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/08/13 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __ONENET_DRIVER_H__
#define __ONENET_DRIVER_H__

#include <string>
#include "SprObserver.h"

class OneNetDriver : public SprObserver
{
public:
    ~OneNetDriver();
    OneNetDriver* GetInstance(ModuleIDType id, const std::string& name);
    int32_t ProcessMsg(const SprMsg& msg) override;

private:
    explicit OneNetDriver(ModuleIDType id, const std::string& name);

private:
    std::string mOneNetAddress;
    uint16_t    mOneNetPort;
};

#endif // __ONENET_DRIVER_H__
