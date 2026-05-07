/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CJConfigParser.cpp
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
#include "CLog.h"
#include "CAtom.h"
#include "CField.h"
#include "CJConfigParser.h"

CJConfigParser::CJConfigParser(const std::string& jText) {
    mJText = jText;
    mpJRoot = cJSON_Parse(jText.c_str());
    if (!mpJRoot) {
        CLOGE("Parse json failed! (%s)", cJSON_GetErrorPtr());
    }
}

CJConfigParser::~CJConfigParser() {
    if (mpJRoot) {
        cJSON_Delete(mpJRoot);
        mpJRoot = nullptr;
    }
}

static std::shared_ptr<CNode> ParseJsonToNode(cJSON* pJson, std::shared_ptr<CNode> pParent) {
    if (!pJson) {
        CLOGE("pJson is nullptr!");
        return nullptr;
    }

    std::shared_ptr<CField> pCurNode = std::make_shared<CField>(pParent);
    if (!pCurNode) {
        CLOGE("pCurNode is nullptr!");
        return nullptr;
    }

    cJSON* pName = cJSON_GetObjectItem(pJson, TEXT_NAME);
    if (cJSON_IsString(pName)) {
        pCurNode->SetName(pName->valuestring);
    }

    cJSON* pType = cJSON_GetObjectItem(pJson, TEXT_TYPE);
    if (cJSON_IsString(pType)) {
        pCurNode->SetType(pType->valuestring);
    }

    if (pCurNode->GetType() != TEXT_DYNAMIC_FIELD
     && pCurNode->GetType() != TEXT_STATIC_FIELD) {
        pCurNode->SetField(false);
        return pCurNode;
    }

    cJSON* pLenRef = cJSON_GetObjectItem(pJson, TEXT_LEN_REF);
    if (cJSON_IsString(pLenRef)) {
        pCurNode->SetLenReference(pLenRef->valuestring);
    }

    cJSON* pLenMode = cJSON_GetObjectItem(pJson, TEXT_LEN_MODE);
    if (cJSON_IsString(pLenMode)) {
        pCurNode->SetLenMode(pLenMode->valuestring);
    }

    cJSON* pChildren = cJSON_GetObjectItem(pJson, TEXT_CHILDREN);
    if (cJSON_IsArray(pChildren)) {
        cJSON *pSubObj = nullptr;
        pCurNode->SetChildNodesTag(TEXT_CHILDREN);
        cJSON_ArrayForEach(pSubObj, pChildren) {
            std::shared_ptr<CNode> pChildNode = ParseJsonToNode(pSubObj, pCurNode);
            if (pChildNode) {
                pCurNode->AddNode(pChildNode);
            }
        }
    }

    cJSON* pChildTemplate = cJSON_GetObjectItem(pJson, TEXT_CHILD_TEMPLATE);
    if (cJSON_IsObject(pChildTemplate)) {
        pCurNode->SetChildNodesTag(TEXT_CHILD_TEMPLATE);
        std::shared_ptr<CNode> pChildNode = ParseJsonToNode(pChildTemplate, pCurNode);
        if (pChildNode) {
            pCurNode->AddNode(pChildNode);
        }
    }

    return pCurNode;
}

std::shared_ptr<CNode> CJConfigParser::CJsonToNode() {
    if (!mpJRoot) {
        CLOGE("mpJRootis nullptr!");
        return nullptr;
    }

    return ParseJsonToNode(mpJRoot, nullptr);
}
