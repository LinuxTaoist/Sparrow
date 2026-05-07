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

CFactory::CFactory() {

}

CFactory::~CFactory() {

}

CFactory& CFactory::GetInstance() {
    static CFactory factory;
    return factory;
}

std::shared_ptr<CNode> CFactory::CreateParserWithJFile(const std::string& path) {
    std::string text;
    int32_t ret = ReadFile(path, text);
    if (ret <= 0) {
        return nullptr;
    }

    return CreateParserWithJString(text);
}

std::shared_ptr<CNode> CFactory::CreateParserWithJString(const std::string& str) {
    CJConfigParser theCfgParser(str);
    return theCfgParser.CJsonToNode();
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

static void PrintNode(const std::shared_ptr<CNode>& pNode, int level) {
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
            PrintNode(pChild, level + 1);
        }
    }
}

void CFactory::PrintAllNodes(const std::shared_ptr<CNode>& pNode) {
    CLOGI("----------------  Print All Nodes  ----------------\n");
    PrintNode(pNode, 0);
    CLOGI("---------------------------------------------------\n");
}
