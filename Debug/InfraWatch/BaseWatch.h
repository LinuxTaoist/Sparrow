/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : BaseWatch.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/09/08
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/09/08 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __BASE_WATCH_H__
#define __BASE_WATCH_H__

#include "InfraCommon.h"

class BaseWatch {
public:
    void Entry();

protected:
    BaseWatch() = default;
    virtual ~BaseWatch() = default;

    virtual void Usage() = 0;
    virtual void Menu(char input) = 0;
};

#endif // __BASE_WATCH_H__
