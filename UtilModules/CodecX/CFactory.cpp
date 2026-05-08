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

std::shared_ptr<CNode> CFactory::CreateParserWithCFile(const std::string& cfgPath) {
    std::string text;
    int32_t ret = ReadFile(cfgPath, text);
    if (ret <= 0) {
        return nullptr;
    }

    return CreateParserWithCString(text);
}

std::shared_ptr<CNode> CFactory::CreateParserWithCString(const std::string& cfgString) {
    CJConfigParser theCfgParser(cfgString);
    return theCfgParser.CJsonToNode();
}

std::shared_ptr<CNode> CFactory::DecodeWithParser(const std::shared_ptr<CNode>& pParser, const std::vector<uint8_t>& bytes) {
    std::shared_ptr<CNode> pData = pParser->Clone();
    if (!pData) {
        CLOGE("Create parser failed!\n");
        return nullptr;
    }

    pData->SetParentNode(nullptr);
    int32_t ret = pData->Decode(bytes);
    return (ret == -1) ? nullptr : pData;
}

int32_t CFactory::DecodeWithCFileAndBFile(const std::string& cfgPath, const std::string& bytesPath) {
    std::shared_ptr<CNode> pParser = CreateParserWithCFile(cfgPath);
    if (!pParser) {
        CLOGE("pParser is nullptr!\n");
        return -1;
    }

    std::vector<uint8_t> hexBytes;
    CUtils::ReadTextToHexVector(bytesPath, hexBytes);

    std::shared_ptr<CNode> pData = DecodeWithParser(pParser, hexBytes);
    PrintDataDetails(pData);
    return 0;
}

int32_t CFactory::ReadFile(const std::string& path, std::string& str) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        return -1;
    }

    file.seekg(0, std::ios::end);
    int32_t size = static_cast<int32_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    str.resize(size);
    file.read(&str[0], size);
    if (!file.good() && !file.eof()) {
        CLOGE("Read %s failed! (%s) \n", path.c_str(), strerror(errno));
        file.close();
        return -1;
    }

    file.close();
    return size;
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
    if (pNode->GetType() == TEXT_TYPE_DFIELD) {
        CLOGI("%s - len_ref : %s\n", indent.c_str(), pField->GetLenReference().c_str());
        CLOGI("%s - len_mode: %s\n", indent.c_str(), pField->GetLenMode().c_str());
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
        auto atom = std::dynamic_pointer_cast<CAtom>(pNode);
        std::string hex = atom->DumpHexValue();
        int size = hex.empty() ? 0 : (hex.size() + 1) / 3;

        CLOGI("%s[%02d-%02d] %s: 0x%s\n", indent.c_str(), start, start+size-1, name.c_str(), hex.c_str());
        offset += size;
        return;
    }

    auto field = std::dynamic_pointer_cast<CField>(pNode);
    auto children = field->GetChildNodes();
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
