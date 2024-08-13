/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : OneNetManager.h
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
#ifndef __ONENET_MANAGER_H__
#define __ONENET_MANAGER_H__

#include <string>
#include "SprObserver.h"

class OneNetManager : public SprObserver
{
public:
    ~OneNetManager();
    static OneNetManager* GetInstance(ModuleIDType id, const std::string& name);

private:
    OneNetManager(ModuleIDType id, const std::string& name);
};


#endif // __ONENET_MANAGER_H__
