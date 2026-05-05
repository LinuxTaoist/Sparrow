/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CFactory.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2026/05/05
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/05/05 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __CFACTORY_H__
#define __CFACTORY_H__

#include <memory>
#include "CNode.h"

class CFactory {
public:
    static CFactory& GetInstance();
    std::shared_ptr<CNode> LoadJsonFile(const std::string& path);
    std::shared_ptr<CNode> LoadJsonString(const std::string& str);

private:
    CFactory();
    ~CFactory();
};

#endif // __CFACTORY_H__
