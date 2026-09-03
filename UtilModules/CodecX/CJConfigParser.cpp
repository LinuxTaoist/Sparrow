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

#define CLOG_TAG "CJCParser"

CJConfigParser::CJConfigParser(const std::string& jText) {
    mJText = jText;
    mpJRoot = cJSON_Parse(jText.c_str());
    if (!mpJRoot) {
        CLOGE("Parse json failed! (%s) \n", cJSON_GetErrorPtr());
    }
}

CJConfigParser::~CJConfigParser() {
    if (mpJRoot) {
        cJSON_Delete(mpJRoot);
        mpJRoot = nullptr;
    }
}

std::shared_ptr<CNode> CJConfigParser::CreateNode(const std::string& type, const std::shared_ptr<CNode>& pParent) {
    std::shared_ptr<CNode> pNode = nullptr;
    if (type.find(TEXT_TYPE_FIELD_SUFFIX) != std::string::npos) {
        pNode = std::make_shared<CField>(pParent);
    } else {
        pNode = std::make_shared<CAtom>(pParent);
    }

    return pNode;
}

std::shared_ptr<CNode> CJConfigParser::ParseJsonToNode(cJSON* pJson, const std::shared_ptr<CNode>& pParent) {
    if (!pJson) {
        CLOGE("pJson is nullptr! \n");
        return nullptr;
    }

    std::shared_ptr<CNode> pCurNode = nullptr;
    cJSON* pType = cJSON_GetObjectItem(pJson, TEXT_TYPE_TAG);
    if (cJSON_IsString(pType)) {
        std::string type = pType->valuestring;
        pCurNode = CreateNode(type, pParent);
        pCurNode->SetType(type);
    }

    if (!pCurNode) {
        CLOGE("pCurNode is nullptr! \n");
        return nullptr;
    }

    cJSON* pName = cJSON_GetObjectItem(pJson, TEXT_NAME_TAG);
    if (cJSON_IsString(pName)) {
        pCurNode->SetName(pName->valuestring);
    }

    cJSON* pMatch = cJSON_GetObjectItem(pJson, TEXT_SWITCH_VALUE_TAG);
    if (cJSON_IsNumber(pMatch)) {
        pCurNode->SetSwitchValue(static_cast<int64_t>(pMatch->valuedouble));
    }

    cJSON* pValue = cJSON_GetObjectItem(pJson, TEXT_VALUE_TAG);
    if (pValue && !pCurNode->IsField()) {
        std::shared_ptr<CAtom> pAtom = std::dynamic_pointer_cast<CAtom>(pCurNode);
        if (pAtom) {
            if (cJSON_IsString(pValue)) {
                pAtom->SetStrValue("", pValue->valuestring);
            } else if (cJSON_IsNumber(pValue)) {
                uint64_t numVal = static_cast<uint64_t>(pValue->valueint);
                pAtom->SetIntValue("", numVal);
            }
        }
    }

    // CLOGD("Node[%s]: type = %s \n", pCurNode->GetName().c_str(), pCurNode->GetType().c_str());
    if (pCurNode->GetType() != TEXT_TYPE_DFIELD &&
        pCurNode->GetType() != TEXT_TYPE_SFIELD) {
        return pCurNode;
    }

    std::shared_ptr<CField> pField = std::dynamic_pointer_cast<CField>(pCurNode);
    cJSON* pLenRef = cJSON_GetObjectItem(pJson, TEXT_LEN_REF_TAG);
    if (cJSON_IsString(pLenRef)) {
        pField->SetLenReference(pLenRef->valuestring);
    }

    cJSON* pLenMode = cJSON_GetObjectItem(pJson, TEXT_LEN_MODE_TAG);
    if (cJSON_IsString(pLenMode)) {
        pField->SetLenMode(pLenMode->valuestring);
        // CLOGD("Node[%s]: lenRef = %s, lenMode = %s \n", pCurNode->GetName().c_str(),
        //                 pField->GetLenReference().c_str(), pField->GetLenMode().c_str());
    }

    cJSON* pLenFormula = cJSON_GetObjectItem(pJson, TEXT_LEN_FORMULA_TAG);
    if (cJSON_IsString(pLenFormula)) {
        pField->SetLenFormula(pLenFormula->valuestring);
        // CLOGD("Node[%s]: lenFormula = %s \n", pCurNode->GetName().c_str(), pField->GetLenFormula().c_str());
    }

    cJSON* pChildren = cJSON_GetObjectItem(pJson, TEXT_CHILDREN_TAG);
    if (cJSON_IsArray(pChildren)) {
        cJSON *pSubObj = nullptr;
        pField->SetChildNodesTag(TEXT_CHILDREN_TAG);
        cJSON_ArrayForEach(pSubObj, pChildren) {
            std::shared_ptr<CNode> pChildNode = ParseJsonToNode(pSubObj, pField);
            if (pChildNode) {
                pField->AddNode(pChildNode);
            }
        }
    }

    cJSON* pChildTemplate = cJSON_GetObjectItem(pJson, TEXT_CHILD_TEMPLATE_TAG);
    if (cJSON_IsObject(pChildTemplate)) {
        pField->SetChildNodesTag(TEXT_CHILD_TEMPLATE_TAG);
        std::shared_ptr<CNode> pChildNode = ParseJsonToNode(pChildTemplate, pField);
        if (pChildNode) {
            pField->AddNode(pChildNode);
        }
    }

    return pField;
}

std::shared_ptr<CNode> CJConfigParser::CJsonToNode() {
    if (!mpJRoot) {
        CLOGE("mpJRootis nullptr! \n");
        return nullptr;
    }

    return ParseJsonToNode(mpJRoot, nullptr);
}
