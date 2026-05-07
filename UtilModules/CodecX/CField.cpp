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
#include "CField.h"

CField::CField(const std::shared_ptr<CNode>& parent)
    : CNode(parent, true) {
}

CField::CField(const CField& field) : CNode(field) {
    mLenReference = field.mLenReference;
    mLenMode = field.mLenMode;
}

CField& CField::operator = (const CField& field) {
    CNode::operator = (field);
    mLenReference = field.mLenReference;
    mLenMode = field.mLenMode;
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

int32_t CField::GetNode(const std::string& name, std::shared_ptr<CNode>& node) {
    const auto it = std::find_if(mChildNodes.begin(), mChildNodes.end(),
        [&](const std::shared_ptr<CNode>& ptr) {
        return (ptr->GetName() == name);
    });

    node = (it != mChildNodes.end()) ? (*it) : nullptr;
    return (it != mChildNodes.end()) ? 0 : -1;
}

std::vector<std::shared_ptr<CNode>> CField::GetChildNodes() {
    return mChildNodes;
}

std::shared_ptr<CNode> CField::Clone() {
    return std::make_shared<CField>(*this);
}

int32_t CField::Decode(const std::vector<uint8_t>& bytes) {
    int32_t ret = 0;

    // 首次解码，重置起始位置
    if (!mParentNode) {
        ResetDePos();
    }

    for (auto& node : mChildNodes) {
        if (node->IsField()) {
            continue;
        }

        ret += node->Decode(bytes);
    }

    return ret;
}

int32_t CField::Encode(std::vector<uint8_t>& bytes) {
    int32_t ret = 0;

    // 首次编码，重置起始位置
    if (!mParentNode) {
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
