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
#include "CAtom.h"

CAtom::CAtom() : CNode(false) {
}

CAtom::CAtom(const CAtom& atom) : CNode(atom) {
    mValue.assign(atom.mValue.begin(), atom.mValue.end());
}

CAtom& CAtom::operator = (const CAtom& atom) {
    CNode::operator = (atom);
    mValue = atom.mValue;
    return *this;
}

CAtom::CAtom(CAtom&& atom)
    : CNode(std::move(atom)),
      mValue(std::move(atom.mValue)) {
}

CAtom& CAtom::operator = (CAtom&& atom) {
    CNode::operator = (std::move(atom));
    mValue = std::move(atom.mValue);
    return *this;
}

CAtom::~CAtom() {
}

CAtom::CAtom(const std::string& name, const std::string& text, const std::vector<uint8_t>& value) {
    mName = name;
    mText = text;
    mValue = value;
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
    mValue.assign(bytes.begin(), bytes.end());
    return 0;
}

int32_t CAtom::Encode(std::vector<uint8_t>& bytes) {
    bytes.insert(bytes.end(), mValue.begin(), mValue.end());
    return 0;
}
