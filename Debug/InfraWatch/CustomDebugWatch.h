/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CustomDebugWatch.h
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
#ifndef __CUSTOM_DEBUG_WATCH_H__
#define __CUSTOM_DEBUG_WATCH_H__

#include "InfraCommon.h"

class CustomDebugWatch
{
public:
    CustomDebugWatch() = default;
    ~CustomDebugWatch() = default;

    char MenuEntry();

private:
    char HandleInputInMenu(char input);
};

extern CustomDebugWatch theCustomDebugWatch;

#endif // __CUSTOM_DEBUG_WATCH_H__
