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
#include <utility>

CNode::CNode(const std::shared_ptr<CNode>& pParent, bool isField)
    : mIsField(isField)
    , mSIndex(-1)
    , mEIndex(-1)
    , mLength(-1)
    , mEndian(CENDIAN_BUTT)
    , mName()
    , mParentNode(pParent) {
}

CNode::CNode(const CNode& node)
    : mIsField(node.mIsField)
    , mSIndex(node.mSIndex)
    , mEIndex(node.mEIndex)
    , mLength(node.mLength)
    , mEndian(node.mEndian)
    , mName(node.mName)
    , mParentNode(node.mParentNode) {
}

CNode& CNode::operator = (const CNode& node) {
    if (this != &node) {
        mIsField = node.mIsField;
        mSIndex = node.mSIndex;
        mEIndex = node.mEIndex;
        mLength = node.mLength;
        mEndian = node.mEndian;
        mName = node.mName;
        mParentNode = node.mParentNode;
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

int32_t CNode::GetSIndex() {
    return mSIndex;
}

int32_t CNode::SetEIndex(int32_t eIndex) {
    return (eIndex >= 0) ? (mEIndex = eIndex, 0) : -1;
}

int32_t CNode::GetEIndex() {
    return mEIndex;
}

int32_t CNode::SetLength(int32_t length) {
    return (length >= 0) ? (mLength = length, 0) : -1;
}

int32_t CNode::GetLength() {
    return mLength;
}

int32_t CNode::SetEndian(CEndianType endian) {
    return ((endian >= CENDIAN_LITTLE) && (endian < CENDIAN_BUTT)) ? (mEndian = endian, 0) : -1;
}

int32_t CNode::GetEndian() {
    return mEndian;
}

int32_t CNode::SetName(const std::string& name) {
    return name.empty() ? -1 : (mName = name, 0);
}

std::string CNode::GetName() {
    return mName;
}

