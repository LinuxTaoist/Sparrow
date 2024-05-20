/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManagerInterface.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://linuxtaoist.gitee.io
 *  @date       : 2024/05/11
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/05/11 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __POWER_MANAGER_INTERFACE_H__
#define __POWER_MANAGER_INTERFACE_H__

class PowerManagerInterface
{
public:
    ~PowerManagerInterface();

    static PowerManagerInterface* GetInstance();
    int PowerOn();
    int PowerOff();

private:
    PowerManagerInterface();

private:
    bool mEnable;
};

#endif // __POWER_MANAGER_INTERFACE_H__
