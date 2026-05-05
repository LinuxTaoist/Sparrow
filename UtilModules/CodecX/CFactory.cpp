/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CFactory.cpp
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
#include "CFactory.h"

CFactory::CFactory() {

}

CFactory::~CFactory() {

}

CFactory& CFactory::GetInstance() {
    static CFactory factory;
    return factory;
}

std::shared_ptr<CNode> CFactory::LoadJsonFile(const std::string& path) {
    return nullptr;
}

std::shared_ptr<CNode> CFactory::LoadJsonString(const std::string& str) {
    return nullptr;
}
