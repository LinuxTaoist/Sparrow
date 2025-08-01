/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SprMsg.cpp
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/11/25
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/11/25 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#include "SprMsg.h"
#include "SprLog.h"
#include "GeneralConversions.h"

#define LOG_TAG "SprMsg"

SprMsg::SprMsg()
{
    Init();
}

SprMsg::SprMsg(const SprMsg& srcMsg)
{
    CopyMsg(srcMsg);
}

SprMsg::SprMsg(uint32_t msgId)
{
    Init();
    mMsgId = msgId;
}

SprMsg::SprMsg(uint32_t to, uint32_t msgId)
{
    Init();
    mTo = to;
    mMsgId = msgId;
}

SprMsg::SprMsg(uint32_t to, const SprMsg& srcMsg)
{
    CopyMsg(srcMsg);
    mTo = to;
}

SprMsg::SprMsg(uint32_t from, uint32_t to, uint32_t msgId)
{
    Init();
    mFrom = from;
    mTo = to;
    mMsgId = msgId;
}

SprMsg::SprMsg(std::string datas)
{
    Init();
    Decode(datas);
}

SprMsg& SprMsg::operator=(const SprMsg &srcMsg)
{
    if (this != &srcMsg) {
        CopyMsg(srcMsg);
    }

    return *this;
}

int32_t SprMsg::CopyMsg(const SprMsg& srcMsg)
{
    mSize = srcMsg.mSize;
    mFrom = srcMsg.mFrom;
    mTo = srcMsg.mTo;
    mMsgId = srcMsg.mMsgId;
    mTag = srcMsg.mTag;
    mBoolValue = srcMsg.mBoolValue;
    mU8Value = srcMsg.mU8Value;
    mI8Value = srcMsg.mI8Value;
    mU16Value = srcMsg.mU16Value;
    mI16Value = srcMsg.mI16Value;
    mU32Value = srcMsg.mU32Value;
    mI32Value = srcMsg.mI32Value;
    mU64Value = srcMsg.mU64Value;
    mI64Value = srcMsg.mI64Value;
    mStringLength = srcMsg.mStringLength;
    mString = srcMsg.mString;
    mU8VecLength = srcMsg.mU8VecLength;
    mU8Vec = srcMsg.mU8Vec;
    mI8VecLength = srcMsg.mI8VecLength;
    mI8Vec = srcMsg.mI8Vec;
    mU16VecLength = srcMsg.mU16VecLength;
    mU16Vec = srcMsg.mU16Vec;
    mI16VecLength = srcMsg.mI16VecLength;
    mI16Vec = srcMsg.mI16Vec;
    mU32VecLength = srcMsg.mU32VecLength;
    mU32Vec = srcMsg.mU32Vec;
    mI32VecLength = srcMsg.mI32VecLength;
    mI32Vec = srcMsg.mI32Vec;
    mU64VecLength = srcMsg.mU64VecLength;
    mU64Vec = srcMsg.mU64Vec;
    mI64VecLength = srcMsg.mI64VecLength;
    mI64Vec = srcMsg.mI64Vec;
    mDataSize = srcMsg.mDataSize;
    mDatas = srcMsg.mDatas;
    mEnFuncs = srcMsg.mEnFuncs;
    mDeFuncs = srcMsg.mDeFuncs;

    return 0;
}

void SprMsg::Init()
{
    mSize = 0;
    mFrom = 0;
    mTo = 0;
    mMsgId = 0;
    mTag = 0;
    mBoolValue = false;
    mU8Value = 0xFF;
    mI8Value = 0x7F;
    mU16Value = 0xFFFF;
    mI16Value = 0x7FFF;
    mU32Value = 0xFFFFFFFF;
    mI32Value = 0x7FFFFFFF;
    mU64Value = 0xFFFFFFFFFFFFFFFF;
    mI64Value = 0x7FFFFFFFFFFFFFFF;
    mStringLength = 0;
    mString.clear();
    mU8VecLength = 0;
    mU8Vec.clear();
    mI8VecLength = 0;
    mI8Vec.clear();
    mU16VecLength = 0;
    mU16Vec.clear();
    mI16VecLength = 0;
    mI16Vec.clear();
    mU32VecLength = 0;
    mU32Vec.clear();
    mI32VecLength = 0;
    mI32Vec.clear();
    mU64VecLength = 0;
    mU64Vec.clear();
    mI64VecLength = 0;
    mI64Vec.clear();
    mDataSize = 0;
    mDatas.clear();

    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_BOOLVALUE, &SprMsg::EncodeBoolValue));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U8VALUE, &SprMsg::EncodeU8Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I8VALUE, &SprMsg::EncodeI8Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U16VALUE, &SprMsg::EncodeU16Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I16VALUE, &SprMsg::EncodeI16Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U32VALUE, &SprMsg::EncodeU32Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I32VALUE, &SprMsg::EncodeI32Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U64VALUE, &SprMsg::EncodeU64Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I64VALUE, &SprMsg::EncodeI64Value));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_STRING, &SprMsg::EncodeString));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U8VEC, &SprMsg::EncodeU8Vec));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I8VEC, &SprMsg::EncodeI8Vec));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U16VEC, &SprMsg::EncodeU16Vec));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I16VEC, &SprMsg::EncodeI16Vec));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U32VEC, &SprMsg::EncodeU32Vec));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I32VEC, &SprMsg::EncodeI32Vec));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U64VEC, &SprMsg::EncodeU64Vec));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I64VEC, &SprMsg::EncodeI64Vec));
    mEnFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_PTR, &SprMsg::EncodeDatas));

    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_BOOLVALUE, &SprMsg::DecodeBoolValue));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U8VALUE, &SprMsg::DecodeU8Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I8VALUE, &SprMsg::DecodeI8Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U16VALUE, &SprMsg::DecodeU16Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I16VALUE, &SprMsg::DecodeI16Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U32VALUE, &SprMsg::DecodeU32Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I32VALUE, &SprMsg::DecodeI32Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U64VALUE, &SprMsg::DecodeU64Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I64VALUE, &SprMsg::DecodeI64Value));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_STRING, &SprMsg::DecodeString));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U8VEC, &SprMsg::DecodeU8Vec));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I8VEC, &SprMsg::DecodeI8Vec));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U16VEC, &SprMsg::DecodeU16Vec));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I16VEC, &SprMsg::DecodeI16Vec));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U32VEC, &SprMsg::DecodeU32Vec));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I32VEC, &SprMsg::DecodeI32Vec));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_U64VEC, &SprMsg::DecodeU64Vec));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_I64VEC, &SprMsg::DecodeI64Vec));
    mDeFuncs.insert(std::make_pair(ESprMsgType::MSG_TYPE_PTR, &SprMsg::DecodeDatas));
}

void SprMsg::Clear()
{
    mSize = 0;
    mTag = 0;
    mString.clear();
    mU8Vec.clear();
    mI8Vec.clear();
    mU16Vec.clear();
    mI16Vec.clear();
    mU32Vec.clear();
    mI32Vec.clear();
    mU64Vec.clear();
    mI64Vec.clear();
    mDatas.clear();
}

// Frame : mMsgId(4) + mTag(4) + length1 +  data1 ... lengthN + dataN
int32_t SprMsg::Decode(std::string& deDatas)
{
    int32_t ret = 0;

    Clear();
    SetSize(deDatas.size());
    DecodeFrom(deDatas);
    DecodeTo(deDatas);
    DecodeMsgId(deDatas);
    DecodeTag(deDatas);
    for (auto i = (int32_t)ESprMsgType::MSG_TYPE_MIN; i < (int32_t)ESprMsgType::MSG_TYPE_MAX; i++) {
        if (mTag & (1 << i)) {
            ESprMsgType type = static_cast<ESprMsgType>(i);
            auto it = mDeFuncs.find(type);

            if (it != mDeFuncs.end()) {
                ((this)->*(it->second))(deDatas);
            } else {
                SPR_LOGW("Not find type: 0x%x!\n", (uint32_t)(type));
            }
        }
    }

    return ret;
}

int32_t SprMsg::Encode(std::string& enDatas) const
{
    int32_t ret = 0;

    EncodeFrom(enDatas);
    EncodeTo(enDatas);
    EncodeMsgId(enDatas);
    EncodeTag(enDatas);
    for (uint32_t i = (uint32_t)ESprMsgType::MSG_TYPE_MIN; i < (uint32_t)ESprMsgType::MSG_TYPE_MAX; i++) {
        if (mTag & (1 << i)) {
            ESprMsgType type = static_cast<ESprMsgType>(i);
            auto it = mEnFuncs.find(type);
            if (it != mEnFuncs.end()) {
                (const_cast<SprMsg*>(this)->*(it->second))(enDatas);
            }
            else {
                SPR_LOGW("Not find type: 0x%x! \n", (uint32_t)type);
            }
        }
    }

    return ret;
}

void SprMsg::SetFrom(uint32_t from)
{
    mFrom = from;
}

void SprMsg::SetTo(uint32_t to)
{
    mTo = to;
}

void SprMsg::SetMsgId(uint32_t msgId)
{
    mMsgId = msgId;
}

void SprMsg::SetBoolValue(bool value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_BOOLVALUE);
    mBoolValue = value;
}

void SprMsg::SetU8Value(uint8_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U8VALUE);
    mU8Value = value;
}

void SprMsg::SetI8Value(int8_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_I8VALUE);
    mI8Value = value;
}

void SprMsg::SetU16Value(uint16_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U16VALUE);
    mU16Value = value;
}

void SprMsg::SetI16Value(int16_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_I16VALUE);
    mI16Value = value;
}

void SprMsg::SetU32Value(uint32_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U32VALUE);
    mU32Value = value;
}

void SprMsg::SetI32Value(int32_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_I32VALUE);
    mI32Value = value;
}

void SprMsg::SetU64Value(uint64_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U64VALUE);
    mU64Value = value;
}

void SprMsg::SetI64Value(int64_t value)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_I64VALUE);
    mI64Value = value;
}

void SprMsg::SetString(const std::string& str)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_STRING);
    mStringLength = str.size();
    mString = str;
}

void SprMsg::SetU8Vec(const std::vector<uint8_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U8VEC);
    mU8VecLength = vec.size();
    mU8Vec.assign(vec.begin(), vec.end());
}

void SprMsg::SetI8Vec(const std::vector<int8_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_I8VEC);
    mI8VecLength = vec.size();
    mI8Vec.assign(vec.begin(), vec.end());
}

void SprMsg::SetU16Vec(const std::vector<uint16_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U16VEC);
    mU16VecLength = vec.size();
    mU16Vec.assign(vec.begin(), vec.end());
}

void SprMsg::SetI16Vec(const std::vector<int16_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_I16VEC);
    mI16VecLength = vec.size();
    mI16Vec.assign(vec.begin(), vec.end());
}

void SprMsg::SetU32Vec(const std::vector<uint32_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U32VEC);
    mU32VecLength = vec.size();
    mU32Vec.assign(vec.begin(), vec.end());
}

void SprMsg::SetI32Vec(const std::vector<int32_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_I32VEC);
    mI32VecLength = vec.size();
    mI32Vec.assign(vec.begin(), vec.end());
}

void SprMsg::SetU64Vec(const std::vector<uint64_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_U64VEC);
    mU64VecLength = vec.size();
    mU64Vec.assign(vec.begin(), vec.end());
}

void SprMsg::SetI64Vec(const std::vector<int64_t>& vec)
{
    mTag |= (1 << (int32_t)ESprMsgType::MSG_TYPE_I64VEC);
    mI64VecLength = vec.size();
    mI64Vec.assign(vec.begin(), vec.end());
}

void SprMsg::EncodeFrom(std::string& enDatas) const
{
    GeneralConversions::ToString(mFrom, enDatas);
}

void SprMsg::EncodeTo(std::string& enDatas) const
{
    GeneralConversions::ToString(mTo, enDatas);
}

void SprMsg::EncodeMsgId(std::string& enDatas) const
{
    GeneralConversions::ToString(mMsgId, enDatas);
}

void SprMsg::EncodeTag(std::string& enDatas) const
{
    GeneralConversions::ToString(mTag, enDatas);
}

void SprMsg::EncodeBoolValue(std::string& enDatas)
{
    enDatas.push_back((uint8_t)mBoolValue);
}

void SprMsg::EncodeU8Value(std::string& enDatas)
{
    enDatas.push_back(mU8Value);
}

void SprMsg::EncodeI8Value(std::string& enDatas)
{
    enDatas.push_back(mI8Value);
}

void SprMsg::EncodeU16Value(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU16Value >> 8));
    enDatas.push_back(0xFF & mU16Value);
}

void SprMsg::EncodeI16Value(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mI16Value >> 8));
    enDatas.push_back(0xFF & mI16Value);
}

void SprMsg::EncodeU32Value(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU32Value >> 24));
    enDatas.push_back(0xFF & (mU32Value >> 16));
    enDatas.push_back(0xFF & (mU32Value >> 8));
    enDatas.push_back(0xFF & mU32Value);
}

void SprMsg::EncodeI32Value(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mI32Value >> 24));
    enDatas.push_back(0xFF & (mI32Value >> 16));
    enDatas.push_back(0xFF & (mI32Value >> 8));
    enDatas.push_back(0xFF & mI32Value);
}

void SprMsg::EncodeU64Value(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU64Value >> 56));
    enDatas.push_back(0xFF & (mU64Value >> 48));
    enDatas.push_back(0xFF & (mU64Value >> 40));
    enDatas.push_back(0xFF & (mU64Value >> 32));
    enDatas.push_back(0xFF & (mU64Value >> 24));
    enDatas.push_back(0xFF & (mU64Value >> 16));
    enDatas.push_back(0xFF & (mU64Value >> 8));
    enDatas.push_back(0xFF & mU64Value);
}

void SprMsg::EncodeI64Value(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mI64Value >> 56));
    enDatas.push_back(0xFF & (mI64Value >> 48));
    enDatas.push_back(0xFF & (mI64Value >> 40));
    enDatas.push_back(0xFF & (mI64Value >> 32));
    enDatas.push_back(0xFF & (mI64Value >> 24));
    enDatas.push_back(0xFF & (mI64Value >> 16));
    enDatas.push_back(0xFF & (mI64Value >> 8));
    enDatas.push_back(0xFF & mI64Value);
}

void SprMsg::EncodeString(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mStringLength >> 24));
    enDatas.push_back(0xFF & (mStringLength >> 16));
    enDatas.push_back(0xFF & (mStringLength >> 8));
    enDatas.push_back(0xFF & mStringLength);
    enDatas.append(mString);
}

void SprMsg::EncodeU8Vec(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU8VecLength >> 24));
    enDatas.push_back(0xFF & (mU8VecLength >> 16));
    enDatas.push_back(0xFF & (mU8VecLength >> 8));
    enDatas.push_back(0xFF & mU8VecLength);
    enDatas.insert(enDatas.end(), mU8Vec.begin(), mU8Vec.end());
}

void SprMsg::EncodeI8Vec(std::string& enDatas) {
    enDatas.push_back(0xFF & (mI8VecLength >> 24));
    enDatas.push_back(0xFF & (mI8VecLength >> 16));
    enDatas.push_back(0xFF & (mI8VecLength >> 8));
    enDatas.push_back(0xFF & mI8VecLength);
    enDatas.insert(enDatas.end(), mI8Vec.begin(), mI8Vec.end());
}

void SprMsg::EncodeU16Vec(std::string& enDatas) {
    enDatas.push_back(0xFF & (mU16VecLength >> 24));
    enDatas.push_back(0xFF & (mU16VecLength >> 16));
    enDatas.push_back(0xFF & (mU16VecLength >> 8));
    enDatas.push_back(0xFF & mU16VecLength);

    for (const auto& value : mU16Vec) {
        enDatas.push_back(0xFF & (value >> 8));
        enDatas.push_back(0xFF & value);
    }
}

void SprMsg::EncodeI16Vec(std::string& enDatas) {
    enDatas.push_back(0xFF & (mI16VecLength >> 24));
    enDatas.push_back(0xFF & (mI16VecLength >> 16));
    enDatas.push_back(0xFF & (mI16VecLength >> 8));
    enDatas.push_back(0xFF & mI16VecLength);

    for (const auto& value : mI16Vec) {
        enDatas.push_back(0xFF & (value >> 8));
        enDatas.push_back(0xFF & value);
    }
}

void SprMsg::EncodeU32Vec(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU32VecLength >> 24));
    enDatas.push_back(0xFF & (mU32VecLength >> 16));
    enDatas.push_back(0xFF & (mU32VecLength >> 8));
    enDatas.push_back(0xFF & mU32VecLength);

    for (uint32_t i = 0; i < (uint32_t)mU32Vec.size(); i++) {
        uint32_t value = mU32Vec[i];
        enDatas.push_back(0xFF & (value >> 24));
        enDatas.push_back(0xFF & (value >> 16));
        enDatas.push_back(0xFF & (value >> 8));
        enDatas.push_back(0xFF & value);
    }
}

void SprMsg::EncodeI32Vec(std::string& enDatas) {
    enDatas.push_back(0xFF & (mI32VecLength >> 24));
    enDatas.push_back(0xFF & (mI32VecLength >> 16));
    enDatas.push_back(0xFF & (mI32VecLength >> 8));
    enDatas.push_back(0xFF & mI32VecLength);

    for (const auto& value : mI32Vec) {
        enDatas.push_back(0xFF & (value >> 24));
        enDatas.push_back(0xFF & (value >> 16));
        enDatas.push_back(0xFF & (value >> 8));
        enDatas.push_back(0xFF & value);
    }
}

void SprMsg::EncodeU64Vec(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mU64VecLength >> 24));
    enDatas.push_back(0xFF & (mU64VecLength >> 16));
    enDatas.push_back(0xFF & (mU64VecLength >> 8));
    enDatas.push_back(0xFF & mU64VecLength);

    for (uint32_t i = 0; i < (uint32_t)mU64Vec.size(); i++) {
        uint64_t value = mU64Vec[i];
        enDatas.push_back(0xFF & (value >> 56));
        enDatas.push_back(0xFF & (value >> 48));
        enDatas.push_back(0xFF & (value >> 40));
        enDatas.push_back(0xFF & (value >> 32));
        enDatas.push_back(0xFF & (value >> 24));
        enDatas.push_back(0xFF & (value >> 16));
        enDatas.push_back(0xFF & (value >> 8));
        enDatas.push_back(0xFF & value);
    }
}

void SprMsg::EncodeI64Vec(std::string& enDatas) {
    enDatas.push_back(0xFF & (mI64VecLength >> 24));
    enDatas.push_back(0xFF & (mI64VecLength >> 16));
    enDatas.push_back(0xFF & (mI64VecLength >> 8));
    enDatas.push_back(0xFF & mI64VecLength);

    for (const auto& value : mI64Vec) {
        for (int32_t i = 7; i >= 0; --i) {
            enDatas.push_back(0xFF & (value >> (i * 8)));
        }
    }
}

void SprMsg::EncodeDatas(std::string& enDatas)
{
    enDatas.push_back(0xFF & (mDataSize >> 24));
    enDatas.push_back(0xFF & (mDataSize >> 16));
    enDatas.push_back(0xFF & (mDataSize >> 8));
    enDatas.push_back(0xFF & mDataSize);
    enDatas.insert(enDatas.end(), mDatas.begin(), mDatas.end());
}

void SprMsg::DecodeFrom(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mFrom) == -1) {
        SPR_LOGE("Decode from Fail!\n");
        return;
    }

    deDatas = deDatas.substr(sizeof(mFrom));
}

void SprMsg::DecodeTo(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mTo) == -1) {
        SPR_LOGE("Decode to Fail!\n");
        return;
    }

    deDatas = deDatas.substr(sizeof(mTo));
}

void SprMsg::DecodeMsgId(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mMsgId) == -1) {
        SPR_LOGE("Decode msgId Fail!\n");
        return;
    }

    deDatas = deDatas.substr(sizeof(mMsgId));
}

void SprMsg::DecodeTag(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mTag) == -1) {
        SPR_LOGE("Decode tag Fail!\n");
        return;
    }

    deDatas = deDatas.substr(sizeof(mTag));
}

void SprMsg::DecodeBoolValue(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mBoolValue)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mBoolValue = (deDatas[0] != 0);
    deDatas = deDatas.substr(sizeof(bool));
}

void SprMsg::DecodeU8Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mU8Value)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mU8Value = deDatas[0] & 0xFF;
    deDatas = deDatas.substr(sizeof(uint8_t));
}

void SprMsg::DecodeI8Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mI8Value)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mI8Value = deDatas[0] & 0xFF;
    deDatas = deDatas.substr(sizeof(int8_t));
}

void SprMsg::DecodeU16Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mU16Value)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mU16Value = ((deDatas[0] & 0xFF) << 8) |
                (deDatas[1] & 0xFF) ;
    deDatas = deDatas.substr(sizeof(uint16_t));
}

void SprMsg::DecodeI16Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mI16Value)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mI16Value = ((deDatas[0] & 0xFF) << 8) |
                (deDatas[1] & 0xFF);
    deDatas = deDatas.substr(sizeof(int16_t));
}

void SprMsg::DecodeU32Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mU32Value)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    uint32_t value = 0;
    for (int32_t i = 0; i < (int32_t)sizeof(mU32Value); i++) {
        value <<= 8;
        value |= (deDatas[i] & 0xFF);
    }
    mU32Value = value;
    deDatas = deDatas.substr(sizeof(mU32Value));
}

void SprMsg::DecodeI32Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mI32Value)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    int32_t value = 0;
    for (int32_t i = 0; i < (int32_t)sizeof(mI32Value); i++) {
        value <<= 8;
        value |= (deDatas[i] & 0xFF);
    }
    mI32Value = value;
    deDatas = deDatas.substr(sizeof(mI32Value));
}

void SprMsg::DecodeU64Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mU64Value)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    uint64_t value = 0;
    for (int32_t i = 0; i < (int32_t)sizeof(mU64Value); i++) {
        value <<= 8;
        value |= (deDatas[i] & 0xFF);
    }
    mU64Value = value;
    deDatas = deDatas.substr(sizeof(mU64Value));
}

void SprMsg::DecodeI64Value(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mI64Value)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    int64_t value = 0;
    for (int32_t i = 0; i < (int32_t)sizeof(mI64Value); i++) {
        value <<= 8;
        value |= (deDatas[i] & 0xFF);
    }
    mI64Value = value;
    deDatas = deDatas.substr(sizeof(mI64Value));
}

void SprMsg::DecodeString(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mStringLength) == -1) {
        SPR_LOGE("Decode string length Fail!\n");
        return;
    }

    if (deDatas.size() < sizeof(mStringLength) + mStringLength) {
        SPR_LOGE("deDatas is invalid! size = %zu, length = %zu\n", deDatas.size(), sizeof(mStringLength) + mStringLength);
        return;
    }

    mString = deDatas.substr(sizeof(mStringLength), mStringLength);
    deDatas = deDatas.substr(sizeof(mStringLength) + mStringLength);
}

void SprMsg::DecodeU8Vec(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mU8VecLength) == -1) {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }

    if (deDatas.size() < sizeof(mU8VecLength) + mU8VecLength) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mU8Vec.assign(deDatas.begin() + sizeof(mU8VecLength), deDatas.begin() + sizeof(mU8VecLength) + mU8VecLength);
    deDatas = deDatas.substr(sizeof(mU8VecLength) + mU8VecLength);
}

void SprMsg::DecodeI8Vec(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mI8VecLength) == -1) {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }

    if (deDatas.size() < sizeof(mI8VecLength) + mI8VecLength) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    mI8Vec.assign(deDatas.begin() + sizeof(mI8VecLength), deDatas.begin() + sizeof(mI8VecLength) + mI8VecLength);
    deDatas = deDatas.substr(sizeof(mI8VecLength) + mI8VecLength);
}

void SprMsg::DecodeU16Vec(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mU16VecLength) == -1) {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }

    if (deDatas.size() < sizeof(mU16VecLength) + mU16VecLength * sizeof(uint16_t)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    for (uint32_t i = 0; i < mU16VecLength; i++) {
        uint16_t value = 0;
        for (uint32_t j = 0; j < sizeof(uint16_t); j++) {
            value <<= 8;
            value |= deDatas[sizeof(mU16VecLength) + i * sizeof(uint16_t) + j] & 0xFF;
        }
        mU16Vec.push_back(value);
    }

    deDatas = deDatas.substr(sizeof(mU16VecLength) + mU16VecLength * sizeof(uint16_t));
}

void SprMsg::DecodeI16Vec(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mI16VecLength) == -1) {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }
    if (deDatas.size() < sizeof(mI16VecLength) + mI16VecLength * sizeof(int16_t)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    for (uint32_t i = 0; i < mI16VecLength; i++) {
        int16_t value = 0;
        for (uint32_t j = 0; j < sizeof(int16_t); j++) {
            value <<= 8;
            value |= deDatas[sizeof(mI16VecLength) + i * sizeof(int16_t) + j] & 0xFF;
        }
        mI16Vec.push_back(value);
    }

    deDatas = deDatas.substr(sizeof(mI16VecLength) + mI16VecLength * sizeof(int16_t));
}

void SprMsg::DecodeU32Vec(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mU32VecLength) == -1) {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }

    if (deDatas.size() < sizeof(mU32VecLength) + mU32VecLength * sizeof(uint32_t)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    for (uint32_t i = 0; i < mU32VecLength; i++) {
        uint32_t value = 0;
        for (uint32_t j = 0; j < sizeof(uint32_t); j++) {
            value <<= 8;
            value |= deDatas[sizeof(mU32VecLength) + i * sizeof(uint32_t) + j] & 0xFF;
        }
        mU32Vec.push_back(value);
    }

    deDatas = deDatas.substr(sizeof(mU32VecLength) + mU32VecLength * sizeof(uint32_t));
}

void SprMsg::DecodeI32Vec(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mI32VecLength) == -1) {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }
    if (deDatas.size() < sizeof(mI32VecLength) + mI32VecLength * sizeof(int32_t)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    for (uint32_t i = 0; i < mI32VecLength; i++) {
        int32_t value = 0;
        for (uint32_t j = 0; j < sizeof(int32_t); j++) {
            value <<= 8;
            value |= deDatas[sizeof(mI32VecLength) + i * sizeof(int32_t) + j] & 0xFF;
        }
        mI32Vec.push_back(value);
    }

    deDatas = deDatas.substr(sizeof(mI32VecLength) + mI32VecLength * sizeof(int32_t));
}

void SprMsg::DecodeU64Vec(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mU64VecLength) == -1) {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }

    if (deDatas.size() < sizeof(mU64VecLength) + mU64VecLength * sizeof(uint64_t)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    for (uint32_t i = 0; i < mU64VecLength; i++) {
        uint64_t value = 0;
        for (uint32_t j = 0; j < sizeof(uint64_t); j++) {
            value <<= 8;
            value |= deDatas[sizeof(mU64VecLength) + i * sizeof(uint64_t) + j] & 0xFF;
        }
        mU64Vec.push_back(value);
    }

    deDatas = deDatas.substr(sizeof(mU64VecLength) + mU64VecLength * sizeof(uint64_t));
}

void SprMsg::DecodeI64Vec(std::string& deDatas)
{
    if (GeneralConversions::ToInteger(deDatas, mI64VecLength) == -1) {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }
    if (deDatas.size() < sizeof(mI64VecLength) + mI64VecLength * sizeof(int64_t)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    for (uint32_t i = 0; i < mI64VecLength; i++) {
        int64_t value = 0;
        for (uint32_t j = 0; j < sizeof(int64_t); j++) {
            value <<= 8;
            value |= deDatas[sizeof(mI64VecLength) + i * sizeof(int64_t) + j] & 0xFF;
        }
        mI64Vec.push_back(value);
    }

    deDatas = deDatas.substr(sizeof(mI64VecLength) + mI64VecLength * sizeof(int64_t));
}

void SprMsg::DecodeDatas(std::string& deDatas)
{
    if (deDatas.size() < sizeof(mDataSize)) {
        SPR_LOGE("deDatas is invalid!\n");
        return;
    }

    if (GeneralConversions::ToInteger(deDatas, mDataSize) == -1) {
        SPR_LOGE("Decode vector size Fail!\n");
        return;
    }

    mDatas.assign(deDatas.begin() + sizeof(mDataSize), deDatas.begin() + sizeof(mDataSize) + mDataSize);
    deDatas = deDatas.substr(sizeof(mDataSize) + mDataSize);
}
