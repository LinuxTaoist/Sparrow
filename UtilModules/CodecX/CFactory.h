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
    std::shared_ptr<CNode> CreateParserWithJFile(const std::string& path);
    std::shared_ptr<CNode> CreateParserWithJString(const std::string& str);
    std::shared_ptr<CNode> DecodeWithParser(const std::shared_ptr<CNode>& pParser, const std::vector<uint8_t>& bytes);

    void PrintConfigDetails(const std::shared_ptr<CNode>& pNode);
    void PrintDataDetails(const std::shared_ptr<CNode>& pNode);

private:
    CFactory();
    ~CFactory();
    int32_t ReadFile(const std::string& path, std::string& str);
};

#endif // __CFACTORY_H__
