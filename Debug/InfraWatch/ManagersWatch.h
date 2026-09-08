/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : ManagersWatch.h
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
#ifndef __MANAGERS_WATCH_H__
#define __MANAGERS_WATCH_H__

#include "BaseWatch.h"

class ManagersWatch : public BaseWatch {
public:
    static ManagersWatch& GetInstance();

private:
    ManagersWatch() = default;
    ~ManagersWatch() override = default;
    void Usage() override;
    void Menu(char input) override;
};

#endif // __MANAGERS_WATCH_H__
