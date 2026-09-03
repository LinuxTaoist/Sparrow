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
#include "CLog.h"
#include <utility>

thread_local int32_t CNode::mDePos = -1;
thread_local int32_t CNode::mEnPos = -1;

CNode::CNode(const std::shared_ptr<CNode>& pParent, bool isField)
    : std::enable_shared_from_this<CNode>()
    , mIsField(isField)
    , mSIndex(-1)
    , mEIndex(-1)
    , mLength(-1)
    , mEndian(CENDIAN_BIG)
    , mName()
    , mType()
    , mHasSwitchValue(false)
    , mSwitchValue(0)
    , mParentNode(pParent) {
}

CNode::CNode(const CNode& node)
    : std::enable_shared_from_this<CNode>()
    , mIsField(node.mIsField)
    , mSIndex(node.mSIndex)
    , mEIndex(node.mEIndex)
    , mLength(node.mLength)
    , mEndian(node.mEndian)
    , mName(node.mName)
    , mType(node.mType)
    , mHasSwitchValue(node.mHasSwitchValue)
    , mSwitchValue(node.mSwitchValue)
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
        mType = node.mType;
        mHasSwitchValue = node.mHasSwitchValue;
        mSwitchValue = node.mSwitchValue;
        mParentNode = node.mParentNode;
    }

    return *this;
}

CNode::~CNode() {
}

bool CNode::IsField() {
    return mIsField;
}

void CNode::ResetDePos(int32_t val) {
    mDePos = val;
}

void CNode::DePosAdd(int32_t offset) {
    mDePos += offset;
}

int32_t CNode::GetDePos() {
    return mDePos;
}

void CNode::ResetEnPos(int32_t val) {
    mEnPos = val;
}

void CNode::EnPosAdd(int32_t offset) {
    mEnPos += offset;
}

int32_t CNode::GetEnPos() {
    return mEnPos;
}

void CNode::SetSIndex(int32_t sIndex) {
    mSIndex = sIndex;
}

int32_t CNode::GetSIndex() {
    return mSIndex;
}

void CNode::SetEIndex(int32_t eIndex) {
    mEIndex = eIndex;
}

int32_t CNode::GetEIndex() {
    return mEIndex;
}

void CNode::SetLength(int32_t length) {
    mLength = length;
}

int32_t CNode::GetLength() {
    return mLength;
}

void CNode::SetEndian(CEndianType endian) {
    mEndian = endian;
}

int32_t CNode::GetEndian() {
    return mEndian;
}

void CNode::SetName(const std::string& name) {
    mName = name;
}

std::string CNode::GetName() {
    return mName;
}

void CNode::SetType(const std::string& type) {
    mType = type;
}

std::string CNode::GetType() {
    return mType;
}

void CNode::SetSwitchValue(int64_t switchValue) {
    mHasSwitchValue = true;
    mSwitchValue = switchValue;
}

bool CNode::HasSwitchValue() const {
    return mHasSwitchValue;
}

int64_t CNode::GetSwitchValue() const {
    return mSwitchValue;
}

std::shared_ptr<CNode> CNode::GetParentNode() {
    return mParentNode.lock();
}

std::shared_ptr<CNode> CNode::GetRootNode() {
    std::shared_ptr<CNode> pRootNode = GetParentNode();
    while (pRootNode && pRootNode->GetParentNode()) {
        pRootNode = pRootNode->GetParentNode();
    }

    return pRootNode;
}

void CNode::SetParentNode(const std::shared_ptr<CNode>& pParent) {
    mParentNode = pParent;
}

std::shared_ptr<CNode> GetNodeByPath(const std::string& path) {
    return nullptr;
}

int32_t CNode::SetStrValue(const std::string& name, const std::string& value) {
    std::vector<uint8_t> tmpValue;
    int32_t ret = CUtils::SToV(value, tmpValue);
    if (ret != -1) {
        ret = SetValue(name, tmpValue);
    }

    return ret;
}

int32_t CNode::GetStrValue(const std::string& name, std::string& value) {
    std::vector<uint8_t> tmpValue;
    int32_t ret = GetValue(name, tmpValue);
    if (ret != -1) {
        ret = CUtils::VToS(tmpValue, value);
    }

    return ret;
}
