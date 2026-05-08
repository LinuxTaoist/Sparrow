/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : CAtom.cpp
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
#include "CLog.h"
#include "CUtils.h"
#include "CAtom.h"

#define CLOG_TAG "CAtom"

CAtom::CAtom(const std::shared_ptr<CNode>& parent)
    : CNode(parent, false) {
}

CAtom::CAtom(const CAtom& atom) : CNode(atom) {
    mValue.assign(atom.mValue.begin(), atom.mValue.end());
}

CAtom& CAtom::operator = (const CAtom& atom) {
    CNode::operator = (atom);
    mValue = atom.mValue;
    return *this;
}

CAtom::CAtom(const std::shared_ptr<CNode>& parent, const std::string& name, const std::vector<uint8_t>& value)
    : CNode(parent, false) {
    mValue = value;
    SetName(name);
}

CAtom::~CAtom() {
}


int32_t CAtom::SetStrValue(const std::string& value) {
    return CUtils::SToV(value, mValue);
}

int32_t CAtom::GetStrValue(std::string& value) {
    return CUtils::VToS(mValue, value);
}

std::shared_ptr<CNode> CAtom::Clone() {
    return std::make_shared<CAtom>(*this);
}

int32_t CAtom::Decode(const std::vector<uint8_t>& bytes) {
    int32_t pos = GetDePos();
    if (pos < 0 || pos >= (int32_t)bytes.size()) {
        CLOGE("Invalid pos! pos = %d, size = %d \n", pos, (int32_t)bytes.size());
        return -1;
    }

    int32_t len = -1;
    std::string type = GetType();
    if (type == TEXT_TYPE_U8 || type == TEXT_TYPE_S8) {
        len = 1;
    } else if (type == TEXT_TYPE_U16 || type == TEXT_TYPE_S16) {
        len = 2;
    } else if (type == TEXT_TYPE_U32 || type == TEXT_TYPE_S32) {
        len = 4;
    } else if (type == TEXT_TYPE_U64 || type == TEXT_TYPE_S64) {
        len = 8;
    } else {
        CLOGE("Invalid type! type = %s \n", type.c_str());
        return len;
    }

    if (pos + len > (int32_t)bytes.size()) {
        CLOGE("Invalid size! size = %d, total = %d \n", pos + len, (int32_t)bytes.size());
        return len;
    }

    std::vector<uint8_t> subBytes(bytes.begin() + pos, bytes.begin() + pos + len);
    mValue.assign(subBytes.begin(), subBytes.end());
    DePosAdd(len);
    CLOGD("Node[%s] [%s] %s -> %s\n", GetName().c_str(), GetType().c_str(),
        CUtils::ToHexString(subBytes).c_str(), CUtils::ToHexString(mValue).c_str());
    return len;
}

int32_t CAtom::Encode(std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), mValue.begin(), mValue.end());
    return 0;
}

std::string CAtom::DumpHexValue() {
    return CUtils::ToHexStringWithSpace(mValue);
}
