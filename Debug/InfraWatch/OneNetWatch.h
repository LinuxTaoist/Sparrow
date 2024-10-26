/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetWatch.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2024/10/24
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2024/10/24 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __ONENET_WATCH_H__
#define __ONENET_WATCH_H__

class OneNetWatch
{
public:
    OneNetWatch() = default;
    ~OneNetWatch() = default;

    char MenuEntry();

private:
    char HandleInputInMenu(char input);
};

extern OneNetWatch theOneNetWatch;

#endif // __ONENET_WATCH_H__
