/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CField.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2025/12/19
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2025/12/19 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include <algorithm>
#include "CLog.h"
#include "CDefine.h"
#include "CAtom.h"
#include "CField.h"

#define CLOG_TAG "CField"

CField::CField(const std::shared_ptr<CNode>& parent)
    : CNode(parent, true) {
}

CField::CField(const CField& field)
    : CNode(field)
    , mLenReference(field.mLenReference)
    , mLenMode(field.mLenMode)
    , mChildNodesTag(field.mChildNodesTag) {
    for (const auto& child : field.mChildNodes) {
        if (child) {
            mChildNodes.push_back(child->Clone());
        }
    }
}

CField& CField::operator = (const CField& field) {
    if (this == &field) {
        return *this;
    }

    CNode::operator = (field);
    mLenReference = field.mLenReference;
    mLenMode = field.mLenMode;
    mChildNodesTag = field.mChildNodesTag;

    mChildNodes.clear();
    for (const auto& child : field.mChildNodes) {
        if (child) {
            mChildNodes.push_back(child->Clone());
        }
    }

    return *this;
}

CField::~CField() {
}

void CField::SetLenReference(const std::string& lenReference) {
    mLenReference = lenReference;
}

std::string CField::GetLenReference() {
    return mLenReference;
}

void CField::SetLenMode(const std::string& lenMode) {
    mLenMode = lenMode;
}

std::string CField::GetLenMode() {
    return mLenMode;
}

void CField::SetChildNodesTag(const std::string& childNodesTag) {
    mChildNodesTag = childNodesTag;
}

std::string CField::GetChildNodesTag() {
    return mChildNodesTag;
}

int32_t CField::AddNode(const std::shared_ptr<CNode>& node) {
    mChildNodes.emplace_back(node);
    return 0;
}

int32_t CField::DelNode(const std::shared_ptr<CNode>& node) {
    std::string target = node->GetName();
    auto it = std::find_if(mChildNodes.begin(), mChildNodes.end(),
        [&](const std::shared_ptr<CNode>& ptr) {
        std::string name = ptr->GetName();
        return (name == target);
    });

    if (it != mChildNodes.end()) {
        mChildNodes.erase(it);
        return 0;
    }

    return -1;
}

std::shared_ptr<CNode> CField::GetNode(const std::string& name) {
    const auto it = std::find_if(mChildNodes.begin(), mChildNodes.end(),
        [&](const std::shared_ptr<CNode>& ptr) {
        return (ptr->GetName() == name);
    });

    std::shared_ptr<CNode> pNode = (it != mChildNodes.end()) ? (*it) : nullptr;
    return pNode;
}

std::vector<std::shared_ptr<CNode>> CField::GetChildNodes() {
    return mChildNodes;
}

std::shared_ptr<CNode> CField::Clone() {
    std::shared_ptr<CField> pClone = std::make_shared<CField>(*this);
    if (!pClone) {
        CLOGE("Node[%s] pClone is nullptr!\n", GetName().c_str());
        return nullptr;
    }

    pClone->RelinkChildren(pClone);
    return pClone;
}

void CField::RelinkChildren(const std::shared_ptr<CField>& pThisField) {
    for (auto& child : mChildNodes) {
        if (!child) {
            continue;
        }

        child->SetParentNode(pThisField);
        std::shared_ptr<CField> pChildField = std::dynamic_pointer_cast<CField>(child);
        if (pChildField) {
            pChildField->RelinkChildren(pChildField);
        }
    }
}

int32_t CField::DecodeStaticField(const std::vector<uint8_t>& bytes) {
    int32_t ret = 0;

    // CLOGD("Node[%s] Decode static field, size = %d \n", GetName().c_str(), (int32_t)mChildNodes.size());
    for (auto& node : mChildNodes) {
        ret += node->Decode(bytes);
    }

    return ret;
}

int32_t CField::DecodeDynamicField(const std::vector<uint8_t>& bytes) {
    std::shared_ptr<CField> pParentNode = std::dynamic_pointer_cast<CField>(GetParentNode());
    if (!pParentNode) {
        CLOGE("Node[%s] pParentNode is nullptr!\n", GetName().c_str());
        return -1;
    }

    std::shared_ptr<CNode> pLenNode = pParentNode->GetNode(GetLenReference());
    std::shared_ptr<CAtom> pLenAtom = std::dynamic_pointer_cast<CAtom>(pLenNode);
    if (!pLenAtom) {
        CLOGE("Node[%s] pLenAtom is nullptr!\n", GetName().c_str());
        return -1;
    }

    int32_t count = 0;
    int32_t ret = pLenAtom->GetIntValue(count);
    if (ret == -1) {
        std::vector<uint8_t> orginBytes;
        pLenAtom->GetVecValue(orginBytes);
        CLOGE("Node[%s] GetIntValue failed! (orgin[%d]: %s) \n", GetName().c_str(),
                    (int32_t)orginBytes.size(), CUtils::ToHexString(orginBytes).c_str());
        return -1;
    }

    if (mChildNodes.empty()) {
        CLOGE("Node[%s] mChildNodes is empty!\n", GetName().c_str());
        return -1;
    }

    // CLOGD("Node[%s] Decode dynamic field, count = %d \n", GetName().c_str(), count);
    std::shared_ptr<CNode> pTmpNode = mChildNodes[0];
    mChildNodes.clear();
    for (int i = 0; i < count; i++) {
        std::shared_ptr<CNode> pCloneNode = pTmpNode->Clone();
        int32_t len = pCloneNode->Decode(bytes);
        if (len == -1) {
            CLOGE("Node[%s] Decode failed!\n", GetName().c_str());
            return -1;
        }

        mChildNodes.emplace_back(pCloneNode);
        ret += len;
    }

    return ret;
}

int32_t CField::Decode(const std::vector<uint8_t>& bytes) {
    int32_t ret = 0;

    // 首次解码，重置起始位置
    if (!GetParentNode()) {
        ResetDePos();
    }

    if (GetType() == TEXT_TYPE_DFIELD) {
        ret += DecodeDynamicField(bytes);
    } else if (GetType() == TEXT_TYPE_SFIELD) {
        ret += DecodeStaticField(bytes);
    } else {
        CLOGE("Node[%s] Invalid type %s!\n", GetName().c_str(), GetType().c_str());
    }

    return ret;
}

int32_t CField::Encode(std::vector<uint8_t>& bytes) {
    int32_t ret = 0;

    // 首次编码，重置起始位置
    if (!GetParentNode()) {
        ResetEnPos();
    }

    for (auto& node : mChildNodes) {
        if (node->IsField()) {
            continue;
        }

        ret += node->Encode(bytes);
    }

    return ret;
}
