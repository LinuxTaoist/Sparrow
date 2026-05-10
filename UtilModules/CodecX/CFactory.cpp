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
#include <fstream>
#include <errno.h>
#include <string.h>
#include "CLog.h"
#include "CAtom.h"
#include "CField.h"
#include "CUtils.h"
#include "CFactory.h"
#include "CJConfigParser.h"

#define CLOG_TAG "CFactory"

CFactory::CFactory() {

}

CFactory::~CFactory() {

}

CFactory& CFactory::GetInstance() {
    static CFactory factory;
    return factory;
}

std::shared_ptr<CNode> CFactory::CreateCfgParserByCfgFile(const std::string& cfgPath) {
    std::string text;
    int32_t ret = CUtils::ReadFile(cfgPath, text);
    if (ret <= 0) {
        return nullptr;
    }

    return CreateCfgParserByCfgString(text);
}

std::shared_ptr<CNode> CFactory::CreateCfgParserByCfgString(const std::string& cfgString) {
    CJConfigParser theCfgParser(cfgString);
    return theCfgParser.CJsonToNode();
}

std::shared_ptr<CNode> CFactory::CreateDataParserByCfgParser(const std::shared_ptr<CNode>& pCfgParser, const std::vector<uint8_t>& bytes) {
    std::shared_ptr<CNode> pDataParser = pCfgParser->Clone();
    if (!pDataParser) {
        CLOGE("Create parser failed!\n");
        return nullptr;
    }

    int32_t ret = pDataParser->Decode(bytes);
    return (ret == -1) ? nullptr : pDataParser;
}

void CFactory::PrintDataDetailsByFiles(const std::string& cfgPath, const std::string& bytesPath) {
    std::shared_ptr<CNode> pCfgParser = CreateCfgParserByCfgFile(cfgPath);
    if (!pCfgParser) {
        CLOGE("pCfgParser is nullptr!\n");
        return;
    }

    std::vector<uint8_t> hexBytes;
    CUtils::ReadTextToHexVector(bytesPath, hexBytes);

    std::shared_ptr<CNode> pDataParser = CreateDataParserByCfgParser(pCfgParser, hexBytes);
    PrintDataDetails(pDataParser);
}

static void PrintCfgNode(const std::shared_ptr<CNode>& pNode, int level) {
    if (!pNode) {
        CLOGE("pNode is nullptr! \n");
        return;
    }

    std::string indent(level * 4, ' ');
    CLOGI("%s - name    : %s\n", indent.c_str(), pNode->GetName().c_str());
    CLOGI("%s - type    : %s\n", indent.c_str(), pNode->GetType().c_str());
    if (!pNode->IsField()) {
        std::shared_ptr<CAtom> pAtom = std::dynamic_pointer_cast<CAtom>(pNode);
        CLOGI("%s - value   : %s\n", indent.c_str(), pAtom->DumpHexValue().c_str());
        return;
    }

    std::shared_ptr<CField> pField = std::dynamic_pointer_cast<CField>(pNode);
    if (pField->GetType() == TEXT_TYPE_DFIELD) {
        CLOGI("%s - len_ref : %s\n", indent.c_str(), pField->GetLenReference().c_str());
        CLOGI("%s - len_mode: %s\n", indent.c_str(), pField->GetLenMode().c_str());
        CLOGI("%s - len_formula: %s\n", indent.c_str(), pField->GetLenFormula().c_str());
    }

    std::vector<std::shared_ptr<CNode>> childNodes = pField->GetChildNodes();
    if (!childNodes.empty()) {
        CLOGI("%s - %s [%d]\n", indent.c_str(),
            pField->GetChildNodesTag().c_str(),
            static_cast<int32_t>(childNodes.size()));

        int32_t i = 0;
        for (auto& pChild : childNodes) {
            CLOGI("  %s - %d     \n", indent.c_str(), i++);
            PrintCfgNode(pChild, level + 1);
        }
    }
}

void CFactory::PrintConfigDetails(const std::shared_ptr<CNode>& pNode) {
    CLOGI("--------------  Print Config Details  -------------\n");
    PrintCfgNode(pNode, 0);
    CLOGI("---------------------------------------------------\n");
}

static void PrintDataNode(const std::shared_ptr<CNode>& pNode, int level, int& offset) {
    if (!pNode) {
        return;
    }

    const std::string indent(level * 4, ' ');
    const std::string& name = pNode->GetName();
    const std::string& type = pNode->GetType();
    int start = offset;

    if (!pNode->IsField()) {
        auto pAtom = std::dynamic_pointer_cast<CAtom>(pNode);
        std::string hex = pAtom->DumpHexValue();
        int size = hex.empty() ? 0 : (hex.size() + 1) / 3;

        CLOGI("%s[%02d-%02d] %s: %s\n", indent.c_str(), start, start+size-1, name.c_str(), hex.c_str());
        offset += size;
        return;
    }

    auto field = std::dynamic_pointer_cast<CField>(pNode);
    auto children = field->GetChildNodes();

    std::string lenMode = field->GetLenMode();
    if (lenMode == TEXT_LEN_MODE_BYTES) {
        std::string hexString;
        for (auto& child : children) {
            auto pAtom = std::dynamic_pointer_cast<CAtom>(child);
            hexString += pAtom->DumpHexValue() + " ";
        }
        CLOGI("%s[%02d - %02d] %s: %s\n", indent.c_str(), start, (int32_t)(start + children.size() - 1), name.c_str(), hexString.c_str());
        offset += children.size();
        return;
    }

    if (type == TEXT_TYPE_DFIELD) {
        CLOGI("%s%s[%zu]\n", indent.c_str(), name.c_str(), children.size());
        for (auto& child : children) {
            PrintDataNode(child, level + 1, offset);
        }
        return;
    }

    CLOGI("%s[%02d] %s\n", indent.c_str(), start, name.c_str());
    for (auto& child : children) {
        PrintDataNode(child, level + 1, offset);
    }
}

void CFactory::PrintDataDetails(const std::shared_ptr<CNode>& pNode) {
    if (!pNode) {
        return;
    }

    CLOGI("===================================================\n");
    CLOGI("               Protocol Parse                      \n");
    CLOGI("===================================================\n");

    int offset = 0;
    PrintDataNode(pNode, 0, offset);

    CLOGI("===================================================\n");
    CLOGI("Total size: %d bytes\n", offset);
    CLOGI("===================================================\n");
}
