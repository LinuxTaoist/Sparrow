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

#include "BaseWatch.h"

class MediatorWatch : public BaseWatch {
public:
    static MediatorWatch& GetInstance();

private:
    MediatorWatch() = default;
    ~MediatorWatch() override = default;
    void Usage() override;
    void Menu(char input) override;

    char ShowMQStatus();
    char ShowFieldDetails();
    char ShowSignalName();
};

#endif // __MEDIATOR_WATCH_H__
