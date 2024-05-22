/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : PowerManagerWatch.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/04/23
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/04/23 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __POWER_MANAGER_WATCH_H__
#define __POWER_MANAGER_WATCH_H__

class PowerManagerWatch
{
public:
    PowerManagerWatch() = default;
    ~PowerManagerWatch() = default;

    char MenuEntry();

private:
    char HandleInputInMenu(char input);
};

extern PowerManagerWatch thePowerManagerWatch;

#endif // __POWER_MANAGER_WATCH_H__
