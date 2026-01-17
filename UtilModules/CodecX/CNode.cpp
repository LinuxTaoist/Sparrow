/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CNode.cpp
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
#include "CNode.h"
#include <algorithm>
#include <utility>

CNode::CNode(bool isField)
    : mIsField(isField)
    , mSIndex(-1)
    , mEIndex(-1)
    , mLength(-1)
    , mEndian(CENDIAN_BUTT)
    , mName()
    , mText()
    , mChildNodes() {
}

CNode::CNode(const CNode& node)
    : mIsField(node.mIsField)
    , mSIndex(node.mSIndex)
    , mEIndex(node.mEIndex)
    , mLength(node.mLength)
    , mEndian(node.mEndian)
    , mName(node.mName)
    , mText(node.mText)
    , mChildNodes() {
    mChildNodes.reserve(node.mChildNodes.size());
    for (const auto& ptr : node.mChildNodes) {
        if (ptr) {
            mChildNodes.emplace_back(ptr->Clone());
        }
    }
}

CNode& CNode::operator=(const CNode& node) {
    if (this != &node) {
        mIsField = node.mIsField;
        mSIndex = node.mSIndex;
        mEIndex = node.mEIndex;
        mLength = node.mLength;
        mEndian = node.mEndian;
        mName = node.mName;
        mText = node.mText;
        mChildNodes.reserve(node.mChildNodes.size());
        for (const auto& ptr : node.mChildNodes) {
            if (ptr) {
                mChildNodes.emplace_back(ptr->Clone());
            }
        }
    }

    return *this;
}

CNode::CNode(CNode&& node)
    : mIsField(std::move(node.mIsField))
    , mSIndex(std::move(node.mSIndex))
    , mEIndex(std::move(node.mEIndex))
    , mLength(std::move(node.mLength))
    , mEndian(std::move(node.mEndian))
    , mName(std::move(node.mName))
    , mText(std::move(node.mText))
    , mChildNodes(std::move(node.mChildNodes)) {
    node.mSIndex = -1;
    node.mEIndex = -1;
    node.mLength = -1;
    node.mEndian = CENDIAN_BUTT;
}

CNode& CNode::operator=(CNode&& node) {
    if (this != &node) {
        mIsField = std::move(node.mIsField);
        mSIndex = std::move(node.mSIndex);
        mEIndex = std::move(node.mEIndex);
        mLength = std::move(node.mLength);
        mEndian = std::move(node.mEndian);
        mName = std::move(node.mName);
        mText = std::move(node.mText);
        mChildNodes = std::move(node.mChildNodes);

        node.mSIndex = -1;
        node.mEIndex = -1;
        node.mLength = -1;
        node.mEndian = CENDIAN_BUTT;
    }
    return *this;
}

CNode::~CNode() {
}

bool CNode::IsField() {
    return mIsField;
}

int32_t CNode::SetSIndex(int32_t sIndex) {
    return (sIndex >= 0) ? (mSIndex = sIndex, 0) : -1;
}

int32_t CNode::GetSIndex(int32_t& sIndex) {
    sIndex = mSIndex;
    return 0;
}

int32_t CNode::SetEIndex(int32_t eIndex) {
    return (eIndex >= 0) ? (mEIndex = eIndex, 0) : -1;
}

int32_t CNode::GetEIndex(int32_t& eIndex) {
    eIndex = mEIndex;
    return 0;
}

int32_t CNode::SetLength(int32_t length) {
    return (length >= 0) ? (mLength = length, 0) : -1;
}

int32_t CNode::GetLength(int32_t& length) {
    length = mLength;
    return 0;
}

int32_t CNode::SetEndian(CEndianType endian) {
    return ((endian >= CENDIAN_LITTLE) && (endian < CENDIAN_BUTT)) ? (mEndian = endian, 0) : -1;
}

int32_t CNode::GetEndian(CEndianType& endian) {
    endian = mEndian;
    return 0;
}

int32_t CNode::SetName(const std::string& name) {
    return name.empty() ? -1 : (mName = name, 0);
}

int32_t CNode::GetName(std::string& name) {
    name = mName;
    return 0;
}

int32_t CNode::SetText(const std::string& text) {
    mText = text;
    return 0;
}

int32_t CNode::GetText(std::string& text) {
    text = mText;
    return 0;
}

int32_t CNode::AddNode(const std::shared_ptr<CNode>& node) {
    mChildNodes.emplace_back(node);
    return 0;
}

int32_t CNode::DelNode(const std::shared_ptr<CNode>& node) {
    std::string target;
    node->GetName(target);
    auto it = std::find_if(mChildNodes.begin(), mChildNodes.end(), [&](const std::shared_ptr<CNode>& ptr) {
        std::string name;
        ptr->GetName(name);
        return name == target;
    });

    if (it != mChildNodes.end()) {
        mChildNodes.erase(it);
        return 0;
    }

    return -1;
}
