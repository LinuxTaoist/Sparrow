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
#include "CField.h"

CField::CField() {
}

CField::CField(const CField& field) : CNode(field) {
    mSerial = field.mSerial;
}

CField& CField::operator = (const CField& field) {
    CNode::operator = (field);
    mSerial = field.mSerial;
    return *this;
}

CField::CField(CField&& field)
    : CNode(field) {
    mSerial = field.mSerial;
}

CField& CField::operator = (CField&& field) {
    CNode::operator = (field);
    mSerial = field.mSerial;
    return *this;
}

CField::~CField() {
}

int32_t CField::SetSerial(int32_t serial) {
    mSerial = serial;
    return 0;
}

int32_t CField::GetSerial(int32_t& serial) {
    serial = mSerial;
    return 0;
}

int32_t CField::SetCapacity(int32_t capacity) {
    mCapacity = capacity;
    return 0;
}

int32_t CField::GetCapacity(int32_t& capacity) {
    capacity = mCapacity;
    return 0;
}

std::shared_ptr<CNode> CField::Clone() {
    return std::make_shared<CField>(*this);
}

int32_t CField::Decode(const std::vector<uint8_t>& bytes) {
    int32_t ret = 0;
    for (auto& node : mChildNodes) {
        ret += node->Decode(bytes);
    }

    return ret;
}

int32_t CField::Encode(std::vector<uint8_t>& bytes) {
    int32_t ret = 0;
    return ret;
}
