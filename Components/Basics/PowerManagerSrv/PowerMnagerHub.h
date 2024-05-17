/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManagerHub.h
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
#ifndef __POWER_MANAGER_HUB_H__
#define __POWER_MANAGER_HUB_H__

#include <string>

class PowerManagerHub
{
public:
    PowerManagerHub(std::string& srvName);
    ~PowerManagerHub();

    virtual void HandleBindCmd(int cmd) = 0;
};

#endif // __POWER_MANAGER_HUB_H__
