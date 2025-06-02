/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : MediatorWatch.h
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
#ifndef __MEDIATOR_WATCH_H__
#define __MEDIATOR_WATCH_H__

class MediatorWatch
{
public:
    MediatorWatch() = default;
    ~MediatorWatch() = default;

    char MenuEntry();

private:
    char HandleInputInMenu(char input);
    char ShowMQStatus();
    char ShowFieldDetails();
    char ShowSignalName();
};

extern MediatorWatch theMediatorWatch;

#endif // __MEDIATOR_WATCH_H__
