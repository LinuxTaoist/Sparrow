/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CJConfigParser.h
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
#ifndef __CJSON_CONFIG_PARSER_H__
#define __CJSON_CONFIG_PARSER_H__

#include <memory>
#include <string>
#include "cJSON.h"
#include "CNode.h"

class CJConfigParser {
public:
    CJConfigParser(const std::string& jText);
    ~CJConfigParser();

    void DumpDetails();
    std::shared_ptr<CNode> CJsonToNode();

private:
    std::string mJText;
    cJSON* mpJRoot;
};

#endif // __CJSON_CONFIG_PARSER_H__
