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
#include <functional>
#include "CNode.h"
#include "CLog.h"

using LogCallback = std::function<void(int level, int line, const char* tag, const char* fmt, va_list ap)>;

class CFactory {
public:
    static CFactory& GetInstance();
    std::shared_ptr<CNode> CreateCfgParserByCfgFile(const std::string& cfgPath);
    std::shared_ptr<CNode> CreateCfgParserByCfgString(const std::string& cfgString);
    std::shared_ptr<CNode> CreateDataParserByCfgParser(const std::shared_ptr<CNode>& pCfgParser, const std::vector<uint8_t>& bytes);

    void RegisterPrintCallback(const LogCallback& callback);
    void PrintConfigDetails(const std::shared_ptr<CNode>& pNode);
    void PrintDataDetails(const std::shared_ptr<CNode>& pCfgParser);
    void PrintDataDetails(const std::shared_ptr<CNode>& pCfgParser, const std::vector<uint8_t>& bytes);
    void PrintDataDetails(const std::string& cfgPath, const std::string& bytesPath);

private:
    CFactory();
    ~CFactory();
    bool GetFrameHeader(const std::shared_ptr<CNode>& pCfgParser, std::vector<uint8_t>& headers);
};

#endif // __CFACTORY_H__
